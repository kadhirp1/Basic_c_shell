// PID: 730324073
// I pledge the COMP211 honor code.
// INSERT HONOR CODE HEADER HERE

// ----------------------------------------------
// These are the only libraries that can be 
// used. Under no circumstances can additional 
// libraries be included.
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include "shell.h"

const char* valid_builtin_commands[] = {"cd", "exit", NULL};


void parse( char* line, command_t* p_cmd ) {
    // TO-DO: COMPLETE THIS FUNCTION BODY
    for (int i=0; i<50; i++){
        p_cmd->argv[i] = malloc(sizeof(char) * 100);
    }
    p_cmd->argc = 0;
    p_cmd->path =malloc(sizeof(char*)*50);
    char lineArr[100];
    strcpy(lineArr,line);
    char* token;
    const char delimit[] = ", \n";
    token = strtok(lineArr, delimit);
    int i=0;

    while (token != NULL){
        strcpy(p_cmd->argv[i], token);
        token = strtok(NULL, delimit);
        i++;
    }

    
    p_cmd->argc = i;
    strcpy(p_cmd->path,p_cmd->argv[0]);
    if (i > 0){
        p_cmd->argv[i] = NULL;
    }
        
    if (is_builtin(p_cmd)){
    }
    else if (!find_fullpath(p_cmd->argv[0],p_cmd)){
        find_fullpath(p_cmd->argv[0],p_cmd);
    }
    else{
        p_cmd->argc = ERROR;
    }
    return;
} // end parse function


int find_fullpath( char* command_name, command_t* p_cmd ) {
    // TO-DO: COMPLETE THIS FUNCTION BODY
    if (p_cmd->argc < 1){
        return 0;
    }
    int exists = FALSE;
    struct stat buffer;
    char* file_or_dir = malloc(256);
    char* file_or_dir_save = file_or_dir;
    const char delimit[] = ":\n";
        
    char *pathCopy = malloc(256);
    char *pathCopySave;// = pathCopy;
    char *pathPointer = pathCopy; 
    pathCopySave = getenv( "PATH" );

    strcpy(pathCopy,pathCopySave);
    char *token = strtok(pathCopy,delimit);
    char fullPathName[100];
    strcpy(file_or_dir,command_name);
    while (token != NULL){
        strcpy(fullPathName, token);
        strcat(fullPathName, "/");
        strcat(fullPathName,file_or_dir);
        exists = stat( fullPathName, &buffer);
        if ( exists == 0 && (S_IFDIR & buffer.st_mode) ) {
           ; //Dir exist
        }
        else if ( exists == 0 && (S_IFREG & buffer.st_mode) ) {
            //File

            strcpy(p_cmd->path,fullPathName);
            break;
        }
        else{
            strcpy(p_cmd->path,command_name);
        }
        token = strtok(NULL, delimit);
        fullPathName[0] = '\0';
    }
    free(pathPointer);
    free(file_or_dir_save);
    return exists;

} // end find_fullpath function


int execute( command_t* p_cmd ) {
    // TO-DO: COMPLETE THIS FUNCTION BODY
    int status = SUCCESSFUL;
    int child_process_status;
    pid_t child_pid;
    
    child_pid = fork();
    if (child_pid == -1){
        status = ERROR;
        return status;
    }
    else if (child_pid == 0){
        int i=0;
        i = p_cmd->argc ++;
        p_cmd->argv[p_cmd->argc] = NULL;
        p_cmd->argv[i] = NULL;
        execv(p_cmd->path, p_cmd->argv);
        perror("Execute terminated with an error condition!\n");
        exit(1);
    }
    else{
         wait(NULL);
    }

    return status;

} // end execute function


int is_builtin( command_t* p_cmd ) {

    int cnt = 0;

    while ( valid_builtin_commands[cnt] != NULL ) {

        if ( equals( p_cmd->path, valid_builtin_commands[cnt] ) ) {

            return TRUE;

        }

        cnt++;

    }

    return FALSE;

} // end is_builtin function


int do_builtin( command_t* p_cmd ) {

    // only builtin command is cd

    if ( DEBUG ) printf("[builtin] (%s,%d)\n", p_cmd->path, p_cmd->argc);

    struct stat buff;
    int status = ERROR;

    if ( p_cmd->argc == 1 ) {

        // -----------------------
        // cd with no arg
        // -----------------------
        // change working directory to that
        // specified in HOME environmental 
        // variable

        status = chdir( getenv("HOME") );

    } else if ( ( stat( p_cmd->argv[1], &buff ) == 0 && ( S_IFDIR & buff.st_mode ) ) ) {


        // -----------------------
        // cd with one arg 
        // -----------------------
        // only perform this operation if the requested
        // folder exists

        status = chdir( p_cmd->argv[1] );

    } 

    return status;

} // end do_builtin function



void cleanup( command_t* p_cmd ) {

    int i=0;
    while ( p_cmd->argv[i] != NULL ) {
        free( p_cmd->argv[i] );
        i++;
    }
    free( p_cmd->argv );
    free( p_cmd->path );	

} // end cleanup function


int equals( char* str1, const char* str2 ) {
    int len[] = {0,0};

    char* b_str1 = str1;
    const char* b_str2 = str2;

    while( (*str1) != '\0' ) { 
        len[0]++;
        str1++;
    }

    while( (*str2) != '\0' ) {
        len[1]++;
        str2++;
    }

    if ( len[0] != len[1] ) {

        return FALSE;

    } else {

        while ( (*b_str1) != '\0' ) {

            if ( tolower( (*b_str1)) != tolower((*b_str2)) ) {

                return FALSE;

            }

            b_str1++;
            b_str2++;

        }

    } 

    return TRUE;


} // end compare function definition
