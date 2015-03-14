/* CSE 306: Sea Wolves Interactive Shell */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "mysyscall.h"
#include "swishHelperFunctions.c"
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "redirHelper.c"
#include "redirection.c"

// Assume no input line will be longer than 1024 bytes
#define MAX_INPUT_BUFF_BUFF 1024


extern char **environ;

int main (int argc, char ** argv, char **envp) {

  /********************************************************************
  Command line parsing begins here
  First check to see if -d is defined, and debugging messages need to be printed.
  ********************************************************************/
  int finished = 0;
  char *prompt = "swish> ";
  char cmd[MAX_INPUT_BUFF_BUFF];
  char cmdCopyPipes[MAX_INPUT_BUFF_BUFF];
//  char tokenCopy[MAX_INPUT_BUFF_BUFF];
  char *EXIT_CMD =  "exit\n";
  char cwd[MAX_INPUT_BUFF_BUFF];

  /********************************************************************
  Setting the commands that the shell recognizes
  ********************************************************************/
  //char *commandsNL[] = { "ls\n","cd\n","pwd\n","printenv\n","putenv\n","cat\n"};
  //char *commands[] = { "ls","cd","pwd","make"};

  /*******************************************************************
  Get the CWD path
  Add a space and a null terminator to the path
  Print the path
  Print the 'swish' prompt
  ********************************************************************/
  int rv;
  char *theCWD = getcwd(cwd,sizeof(cwd));
  char *cursor;
  cursor = theCWD + strlen(theCWD);
  *cursor = ' ';
  cursor++;
  *cursor = '\0';
  cursor =NULL ;

  while (!finished) {
    rv = write(1, theCWD, strlen(theCWD));
    rv = write(1, prompt, strlen(prompt));
    char last_char;
    int count;
    /********************************************************************
    Removing this should make the only exit occur from the proper exit.
    IE typing 'exit'.
    *********************************************************************
    if (!rv) {
    finished = 1;
    break;
  } Ends while loop */


  /* read and parse the input put it into CMD */
  for(rv = 1, count = 0, cursor = cmd, last_char = 1;rv && (++count < (MAX_INPUT_BUFF_BUFF-1)) && (last_char != '\n'); cursor++) {
    rv = read(0, cursor, 1);
    last_char = *cursor;
  }
  *cursor = '\0';

  if(!strncmp(cmd,EXIT_CMD,4)) {
    finished = 1;
    break;
    return 0;
  }

  //test recursive piping here
strncpy(cmdCopyPipes, cmd, strlen(cmd));
int i;
for(i = 0; i < strlen(cmdCopyPipes); i++) {
  if(cmdCopyPipes[i] == '\n') {
    cmdCopyPipes[i] = '\0';
    break;
  }
}
if(*cmdCopyPipes != 0) {
  redirControl(cmdCopyPipes);
}
  memset(cmd,'\0',MAX_INPUT_BUFF_BUFF);
} /* End while */
return 0;
}
