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
  char cwd[MAX_INPUT_BUFF_BUFF];
  bool pipeBool = false;

//execScript("testScript.sh");
  char *extractSh;
  if (argv[1] != NULL){

    extractSh = argv[1];
    extractSh += strlen(argv[1])-3;
    if(!strncmp(extractSh,".sh",3)){

      execScript(argv[1]);
      exit(0);
    }
  }


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
  rv = write(1, theCWD, strlen(theCWD));
  rv = write(1, prompt, strlen(prompt));


  while (!finished) {
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
  int rv = -1;
  if ((rv = checkForCd(cmd)) == 0) {;}
  else if ((rv = checkForExit(cmd)) == 0) {;}
  else if ((rv = checkForSet(cmd)) == 0) {;}
  else if ((rv = checkForWolfie(cmd)) == 0) {;}
  else{

  //test recursive piping here
strncpy(cmdCopyPipes, cmd, strlen(cmd));
int i;
for(i = 0; i < strlen(cmdCopyPipes); i++) {
  if(cmdCopyPipes[i] == '|') {
    pipeBool = true;
  }
  if(cmdCopyPipes[i] == '\n') {
    cmdCopyPipes[i] = '\0';
    break;
  }
}

if(*cmdCopyPipes != 0 && pipeBool) {
  redirControl(cmdCopyPipes);
}
else {
  write(1, "no\n", 3);
  redirOnly(cmdCopyPipes);
}
}
  theCWD = getcwd(cwd,sizeof(cwd));
  char *cursor;
  cursor = theCWD + strlen(theCWD);
  *cursor = ' ';
  cursor++;
  *cursor = '\0';
  cursor =NULL ;
  rv = write(1, theCWD, strlen(theCWD));
  rv = write(1, prompt, strlen(prompt));
  memset(cmd,'\0',MAX_INPUT_BUFF_BUFF);
  pipeBool = false;
} /* End while */
return 0;
}
