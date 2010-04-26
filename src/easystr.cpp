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

#include <windows.h>

//simple string functions

//removes right spaces
char*rtrim(char*c){
	char*ns=c;
	char*space=" \t\r\n";
	for(int i=0;c[i];i++)
		if(!strchr(space,c[i]))
			ns=c+i+1;
	ns[0]=0;
	return c;
}

//returns pointer to the first non-space character of the string (char*)
char * ltrim(char*c){
	while(c[0] && (c[0]==' ' || c[0]=='\t'))c++;
	return c;
}


