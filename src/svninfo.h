/*
 * Copyright 2010 Dmitry Y Lukyanov
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/**
 * this class defines simple class to hold svn object list
 */

 
 
#include "mstring.h"

#ifndef __SVNINFO_H__
#define __SVNINFO_H__
//initial size
#define __SVNINFO_H__INITSIZE	5000



typedef struct {
	int        hash;      //simple hash
	char       *path;     //relative path to file
	long       rev;       //revision 
	char       *owner;    //object owner (locker)
}SVNINFOITEM;


class svninfo {
	private:
		SVNINFOITEM* ptr;
		long size;         
		long count;
		mstring*buf;
		
		int hash(char*c){
			int h=0;
			char*ch="_";
			for (int i = 0; c[i]; i++) {
				ch[0]=c[i];
				CharLowerBuff(ch,1);
				h = 31*h + ch[0];
			}
			return h;
		}
		/** returns relative path of the _path comparing to _root */
		char * relativePath(char*_root,char*_path){
			int len=strlen(_root); //length of the _root must be less or equal to _path
			if(len>0) {
				if(_root[len-1]=='\\' || _root[len-1]=='/')len--;
				if( CompareString(LOCALE_USER_DEFAULT,NORM_IGNORECASE,_root,len,_path,len)==2 ){
					if(!_path[len]) return _path+len;
					if(_path[len]=='\\' || _path[len]=='/' )return _path+len+1;
				}
			}
			//just return the full path
			return _path;
		}
		
		
	public:
		
		svninfo(){
			size  = __SVNINFO_H__INITSIZE;
			count = 0;
			ptr   = new SVNINFOITEM[size];
			memset( ptr, 0, size*sizeof(SVNINFOITEM) );
			buf=new mstring();
		}
		
		~svninfo(){
			delete buf;
			reset();
			delete []ptr;
			ptr=NULL;
			count=0;
			size=0;
		}
		/**
		 * adds to the end of the list an item
		 * will not check if item already exists 
		 * @param _root: the base directory normally it's a root of work directory (used to calculate relative path)
		 * @param _path: the path to the element we want to add 
		 * @param _name: the name of the element we want to add (could be empty if _path contains the full path) 
		 * @param _rev : the revision of the element
		 * @param _owner: the lock owner of the element
		 */
		void add(char*_root,char*_path,char*_name,long _rev,char*_owner){
			if(count+1>=size){
				//reallocate
				SVNINFOITEM *ptr_old=ptr;
				long size_old=size;
				size=size+size/2;
				ptr=new SVNINFOITEM[size];
				memset( ptr, 0, size*sizeof(SVNINFOITEM) );
				memcpy(ptr, ptr_old, size_old*sizeof(SVNINFOITEM));
				delete []ptr_old;
			}
			_path=relativePath(_root,_path);
			
			ptr[count].path=buf->set(_path)->addPath(_name)->c_copy();
			ptr[count].rev=_rev;
			ptr[count].owner=buf->set(_owner)->c_copy();
			ptr[count].hash=hash(ptr[count].path);
			count++;
		}
		
		/** reset all the values */
		void reset() {
			for(int i=0;i<count;i++){
				if(ptr[i].path)  delete []ptr[i].path;
				ptr[i].path=NULL;
				ptr[i].rev=0;
				if(ptr[i].owner) delete []ptr[i].owner;
				ptr[i].owner=NULL;
			}
			count=0;
		}
		
		void print(SVNINFOITEM*e){
			if(!e)printf("(null)\n");
			else printf("%s, %i, %s\n",e->path, e->rev, e->owner);
		}
		
		/** returns element count */
		int getCount(){
			return count;
		}
		
		/** returns element by index */
		SVNINFOITEM* get(int i){
			return &ptr[i];
		}
		
		/** returns svn element by relative path */
		SVNINFOITEM* get(char*_path){
			int h=hash(_path);
			for(int i=0;i<count;i++) {
				if( ptr[i].hash==h ){
					if ( !lstrcmpi( _path, ptr[i].path ) )return &ptr[i];
				}
			}
			return NULL;
		}
		
		/** returns svn element by absolute path with root specified */
		SVNINFOITEM* get(char*_root,char*_path){
			_path=relativePath(_root,_path);
			int h=hash(_path);
			for(int i=0;i<count;i++) {
				if( ptr[i].hash==h ){
					if ( !lstrcmpi( _path, ptr[i].path ) )return &ptr[i];
				}
			}
			return NULL;
		}
	
};	


#endif
