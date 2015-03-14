/* CSE 306: Sea Wolves Interactive Shell */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "mysyscall.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "redirHelper.c"
#include "swishHelperFunctions.c"
//include "piping.c"

// Assume no input line will be longer than 1024 bytes
#define MAX_INPUT_BUFF_BUFF 1024

extern char **environ;

int main (int argc, char ** argv, char **envp) {

  /********************************************************************
  Command line parsing begins here
  First check to see if -d is defined, and debugging messages need to be printed.
  ********************************************************************/
  //int flagD;
  //flagD  = findFlagD(argv, argc);
  //int returnScript = 0;
  //returnScript =  execCLscript(argv,argc);
  //execScript("testScript.sh");
  //printf("Return script : %d \n",returnScript);
  //printWolfie();
  int finished = 0;
  char *prompt = "swish> ";
  char cmd[MAX_INPUT_BUFF_BUFF];
  char cmdCopyPipes[MAX_INPUT_BUFF_BUFF];
  char cwd[MAX_INPUT_BUFF_BUFF];
  int rv;
  char *theCWD = getcwd(cwd,sizeof(cwd));
  char *cursor;
  cursor = theCWD + strlen(theCWD);
  *cursor = ' ';
  cursor++;
  *cursor = '\0';
  cursor =NULL ;

  rv = write(1, theCWD, strlen(theCWD));
  rv = write(1, prompt, strlen(prompt));

  while (!finished) {
    char last_char;
    int count;
  /* read and parse the input put it into CMD */
    for(rv = 1, count = 0, cursor = cmd, last_char = 1;rv && (++count < (MAX_INPUT_BUFF_BUFF-1)) && (last_char != '\n'); cursor++) {
      rv = read(0, cursor, 1);
      last_char = *cursor;
    }
    *cursor = '\0';
    checkForCd(cmd);
    checkForExit(cmd);
    //test recursive piping here
    strncpy(cmdCopyPipes, cmd, strlen(cmd));
    int i;
    for(i = 0; i < strlen(cmdCopyPipes); i++) {
      if(cmdCopyPipes[i] == '\n') {
        cmdCopyPipes[i] = '\0';
        break;
      }
    }
    //checkForSet(cmd);
    redirControl(cmdCopyPipes);
    /*******************************************************************
    Print the CWD
    Print the prompt
    ********************************************************************/
    theCWD = getcwd(cwd,sizeof(cwd));
    cursor = theCWD + strlen(theCWD);
    *cursor = ' ';
    cursor++;
    *cursor = '\0';
    cursor =NULL ;
    rv = write(1, theCWD, strlen(theCWD));
    rv = write(1, prompt, strlen(prompt));
    memset(cmd,'\0',MAX_INPUT_BUFF_BUFF);
  } /* End while */
return 0;
}
