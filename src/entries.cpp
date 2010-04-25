#include <windows.h>
#include <stdio.h>
#include "easystr.h"
#include "entries.h"

//functions to parse .svn/entries file

bool entries_scan_old(char*entries,ENTRYSCANCALLBACK callback,void*udata){
	SVNENTRY e;
	char buf[ES_MAX_LEN];
	char *c=NULL;
	char *p=NULL;
	long count=0;
	
	bool want_continue=true;
	int status=-1;  //-1:not init; 0:new item; 1:old item ; 2:end
	
	FILE*fent=fopen(entries,"rt");
	if(fent){
		while(fgets( buf, ES_MAX_LEN, fent )){
			c=rtrim(ltrim(buf));
			if(!strncmp(c,"<entry",6))status=0;
			if(!strncmp(c,"</wc-entries>",13))status=2;
			if(status==0||status==2){
				//process entry
				if(count>0){
					if(!e.repository[0] && !e.name[0]){
						//shit! we are in the version lower then 1.3.x
						strcpy(e.repository,e.url);
						char *c=strstr(e.repository,"/repos/");
						if(c){
							c+=7;
							c=strchr(c,'/');
							if(c)c[0]=0;
						}
						if(!c)e.repository[0]=0;
					}
					want_continue=callback(&e,udata);
				}
				//clear values
				memset( &e, 0, sizeof(e) );
				count++;
				if(status==0)status=1;
				if(status==2)break;
			}else if(status==1){
				if(!strncmp(c,"committed-rev=\"",15)){
					c+=15;
					p=strchr(c,'"');
					if(p)p[0]=0;
					e.revision=atol(c);
				}else if(!strncmp(c,"name=\"",6)){
					c+=6;
					p=strchr(c,'"');
					if(p)p[0]=0;
					strncpy(e.name,c,MAX_PATH);
					e.name[MAX_PATH]=0;
				}else if(!strncmp(c,"kind=\"",6)){
					c+=6;
					p=strchr(c,'"');
					if(p)p[0]=0;
					strncpy(e.kind,c,ES_SIMPLE_LEN);
					e.kind[ES_SIMPLE_LEN]=0;
				}else if(!strncmp(c,"url=\"",5)){
					c+=5;
					p=strchr(c,'"');
					if(p)p[0]=0;
					strncpy(e.url,c,ES_MAX_LEN);
					e.url[ES_MAX_LEN]=0;
				}else if(!strncmp(c,"repos=\"",7)){
					c+=7;
					p=strchr(c,'"');
					if(p)p[0]=0;
					strncpy(e.repository,c,ES_MAX_LEN);
					e.repository[ES_MAX_LEN]=0;
				}else if(!strncmp(c,"deleted=\"true\"",14)){
					strcpy(e.scedule,"delete");
				}
			}
		}
		fclose(fent);
		return true;
	}
	return false;
}




bool entries_scan(char*entries,ENTRYSCANCALLBACK callback,void*udata){
	char buf[ES_MAX_LEN];
	SVNENTRY e;
	
	bool want_continue=true;
	int status=0; //0: not init or new; 1:line1; ...
	
	FILE*fent=fopen(entries,"rt");
	if(fent){
		if( fgets( buf, ES_MAX_LEN, fent ) ){
			if( atol(rtrim(buf))<4 ){
				//this is an old xml format
				fclose(fent);
				return entries_scan_old(entries,callback,udata);
			}
		}
		//new format
		while(want_continue && fgets( buf, ES_MAX_LEN, fent )){
			rtrim(buf);
			status++;
			if(!strcmp(buf,"\f"))status=0;
			switch(status){
				case 0:
					//end of entry
					want_continue=callback(&e,udata);
				case 1: //name
					//start of the new entry
					//reset the previous entry values
					memset( &e, 0, sizeof(e) );
					//
					strncpy(e.name,buf,MAX_PATH);
					e.name[MAX_PATH]=0;
					break;
				case 2: //kind: file, dir
					strncpy(e.kind,buf,ES_SIMPLE_LEN);
					break;
				case 4: //url
					strncpy(e.url,buf,ES_MAX_LEN);
					break;
				case 5: //repository
					strncpy(e.repository,buf,ES_MAX_LEN);
					break;
				case 6: //scedule: delete, add, replace
					strncpy(e.scedule,buf,ES_SIMPLE_LEN);
					break;
				case 10: //revision
					e.revision=atol( buf );
					break;
				default:
					//do nothing
					break;
			}
		}
		fclose(fent);
		return true;
	}
	return false;
	
}

bool _entries_repository_callback(SVNENTRY*e,void*udata){
	if(!e->name[0] && !strcmp(e->kind,"dir") ){
		strcpy( (char*)udata, e->repository );
		return false; //don't continue
	}
	return true;
}

//returns url of the repository in buf
//true on success
bool entries_repository(char*entries,char*buf,int buflen){
	char lbuf[ES_MAX_LEN+1];
	bool ret=entries_scan(entries, &_entries_repository_callback, (void*)lbuf );
	strncpy(buf,lbuf,buflen);
	buf[buflen-1]=0;
	return ret;
}


