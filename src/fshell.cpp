
#include "fshell.h"
#include <string.h>
#include "c3.h"
#include "LittleFS.h"

char path[ 80] = "/";


void resolvePath( char* result, char* path, char* input){
  char buf[80];
//  char* part;
  int argc = 0;
  char* argv[10];
  char* token;
  char* rest = buf;
  
  *result = '\0';
  strcpy( buf, path);
  strcat( buf,"/");

  if( *input == '/') strcpy( buf, input);
  else strcat( buf, input);
  
  while( (token = strtok_r( rest, "/", &rest))){
    argv[ argc] = token;
    if( strcmp( token, ".") != 0){
      if( strcmp( token, "..") == 0){
        argc--;
        if( argc < 0) argc = 0;
      } else {
        argc++;
      }
    }
  }

  if( argc == 0){
    strcpy( result, "/");
  } else {
    for( int i =0; i<argc; i++){
      strcat( result, "/");
      strcat( result, argv[ i]);
    }
  }
}

int checkDir( char* path){
  int error = 0;
  File file = LittleFS.open( path, "r");
  if( file){
    if( file.isFile()){
      return 1;
    }
    file.close();
  }  
  return error;
}

void printDir( char* path, int m){
  char* pos;
  char fname[ 80];

  Dir dir = LittleFS.openDir( path);
  while(dir.next()) {
    dir.fileName().toCharArray( fname, sizeof( fname));
    if (dir.isFile()) {
      if( m == 0){
        printString( fname);
      } else {
        pos = strchr( fname, '#');
        if( pos) *pos = '\0';
        printString( fname);
      }
    }
    if (dir.isDirectory()) {
        printString( fname); printChar( '/');
      // recursive file listing inside new directory
//      listAllFilesInDir(dir_path + dir.fileName() + "/");
    }
    printString("\r\n");
  }
}

int handle_LS( char* rest){
  int error = 0;
  char newPath[80];
  
  char* token = strtok_r(rest, " ", &rest);
  if( ! token) resolvePath( newPath, path, "");
  else  resolvePath( newPath, path, token);

  printDir( newPath, 0);
  return error;
}


int handle_PWD(){
  printString( path);
  return 0;
}

int handle_CD( char* rest){
  int error = 0;
  char newPath[80];
  
  char* token = strtok_r(rest, " ", &rest);
  if( ! token) return error;
  resolvePath( newPath, path, token);

  error = checkDir( newPath);
  if( error == 0) strcpy( path, newPath);

  return error;
}

int handle_MKDIR( char* rest){
  int error = 0;
  char newPath[80];
  
  char* token = strtok_r(rest, " ", &rest);
  if( ! token) return 1;
//  c_puts( token); c_putch(':');
  resolvePath( newPath, path, token);
//  c_puts( newPath); newline();

  if( ! LittleFS.mkdir( newPath)) error = 1;
 
  return error;
}

int handle_RMDIR( char* rest){
  int error = 0;
  char newPath[80];
  
  char* token = strtok_r(rest, " ", &rest);
  if( ! token) return 1;
  resolvePath( newPath, path, token);

  if( !LittleFS.rmdir( newPath)) error = 1;

  return error;
}

int handle_DELETE( char* rest){
  int error = 0;
  char newPath[80];
  
  char* token = strtok_r(rest, " ", &rest);
  if( ! token) return 1;
  resolvePath( newPath, path, token);
  
  if( ! LittleFS.remove( newPath)) error = 1;

  return error;
}

int handle_RENAME( char* rest){
  int error = 0;
  char oldPath[80];
  char newPath[80];
  
  char* token = strtok_r(rest, " ", &rest);
  if( ! token) return 1;
  resolvePath( oldPath, path, token);

  token = strtok_r(rest, " ", &rest);
  if( ! token) return 1;
  resolvePath( newPath, path, token);

  if( ! LittleFS.rename( oldPath, newPath)) error = 1;

  return error;
}
