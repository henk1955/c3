
#ifndef __FSHELL_H__
#define __FSHELL_H__

int handle_LS( char* rest);
int handle_PWD();
int handle_CD( char* rest);
int handle_MKDIR( char* rest);
int handle_RMDIR( char* rest);
int handle_DELETE( char* rest);
int handle_RENAME( char* rest);


#endif