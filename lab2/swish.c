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
//#include "piping.c"

// Assume no input line will be longer than 1024 bytes
#define MAX_INPUT_BUFF_BUFF 1024

typedef enum {false, true} bool; //boolean enumerations

extern char **environ;

int main (int argc, char ** argv, char **envp) {

  /********************************************************************
  Command line parsing begins here
  First check to see if -d is defined, and debugging messages need to be printed.
  ********************************************************************/

  putenv("SomeVariable=SomeValue");
  int finished = 0;
  char *prompt = "swish> ";
  char cmd[MAX_INPUT_BUFF_BUFF];
  char tokenBuff[MAX_INPUT_BUFF_BUFF];
  char cmdCopyPipes[MAX_INPUT_BUFF_BUFF];
//  char tokenCopy[MAX_INPUT_BUFF_BUFF];
  char cmdTemp[MAX_INPUT_BUFF_BUFF];
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
  bool inRedir, outRedir;

  rv = write(1, theCWD, strlen(theCWD));
  rv = write(1, prompt, strlen(prompt));

  while (!finished) {
    char last_char;
    int count;
    //int iterator =0;
    int pid=-1;

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
  strcpy(tokenBuff,cmd);
  char *tokenArr[50];
  char *tokenArgs[50];
  int counter = 0;
  int counterArgs =0;
  int flag =0;
  char* token = strtok(tokenBuff, " ");

  tokenArgs[counterArgs++] = token;
  while (token) {
    if (!strncmp(token,"$",1)){
      flag = 1;
      char *nlRemove = token;
      token++;
      nlRemove+=strlen(token);
      *nlRemove = '\0'; //printf("getenv : %s \n",getenv(token));
      tokenArgs[counterArgs++] = getenv(token);
    }//env variables
    if (!strncmp(token,"-",1)){
      flag = 1;
      tokenArgs[counterArgs++] = token;
    } else
    if (!strncmp(token,">",1) || !strncmp(token, "<",1) || !strncmp(token, "|", 1)){
      if(strlen(token) == 1){
        token = strtok(NULL, " ");
      } //if char* length = 1
      else {
      } //else char* length > 1
    }//if first char = >
    else {
      tokenArr[counter++] = token;
    } // else
    token = strtok(NULL, " ");
  }//while

  int index = 0;
  for(index = 0; tokenArr[counter - 1][index] != '\0'; index++) {
    if(tokenArr[counter - 1][index] == '\n') {
      tokenArr[counter - 1][index] = '\0';
      break;
    }
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

execute_only_pipes(cmdCopyPipes);
/*  write(1, "test", 5);
  char** pipeTokens;
  strcpy(tokenCopy,cmd);
  pipeTokens = strsplit(tokenCopy, '|');
  int pIndex = 0;
  char* const* cmds[10][50];

  char a1[] = {"hello\n"};
  char* a2[10];
  char** a3[6];
  a2[1] = a1;
  a3[0] = *a2[1];

  write(1, a3[0], 6);

  write(1, *cmds[2], strlen(**cmds[2]));
  while(pipeTokens[pIndex] != NULL) {
  //x`  removeSpaces(pipeTokens[pIndex], cmds[pIndex], pIndex);
    pIndex++;
  }
*/
//    char* cmd1[] = { "ls" , NULL };
  //  char* cmd2[] = { "grep", ".txt", NULL };
  //  char* more[] = { "wc", "-l", NULL };
  ///  char* const* cmds[] = { cmd1, cmd2, more, NULL };
//  pipeline(pipeTokens, 0, STDIN_FILENO);
//  exec_pipeline(*cmds, 0, STDIN_FILENO);
  //recursivePipe(pipeTokens, 1, STDIN_FILENO);
  //free(pipeTokens);
  write(1, "done\n", 5);
//  return 0;
  if (flag == 1){
    for(index = 0; tokenArgs[counterArgs - 1][index] != '\0'; index++) {

      if(tokenArgs[counterArgs - 1][index] == '\n') {
        tokenArgs[counterArgs - 1][index] = '\0';
        break;
      }
    }
  }

  tokenArr[counter] = NULL;
  tokenArgs[counterArgs] = NULL;

  //tokenArgs[1] = "-d";
  /********************************************************************
  Check the CMD string to see if the input matches any of the commands
  found in the commandsNL array.
  ********************************************************************/
  //for(iterator= 0; iterator < 4; iterator++){

  /********************************************************************
  If the command is in the array, this line returns 0 so we ! to enter
  ********************************************************************/
  //if (!strncmp(commandsNL[iterator],cmd,2)){


  /********************************************************************
  IF the command is CD
  ********************************************************************/
  if (strncmp("cd\n",cmd,2) == 0){
    cursor = cmd; //Make a cursor to the CMD string
    cursor += 3; // Move the cursor past 'cd ' onto the argument for CD
    strcpy( cmdTemp , cursor ); // Copy the value at location into the cmdTemp string (this now holds CD arguments)
    cursor = cmdTemp; // The copied over value still has '\n' so we make a cursor it
    cursor += strlen(cmdTemp)-1; // Move the cursor to the end -1 (The location of the '\n')
    *cursor = '\0';  // Replace the '\n' with Null terminator
    if (strcmp( cmd , "cd\n") == 0){ // Base case CD needs to implement root CD
      chdir(getenv("HOME"));
      theCWD = getcwd (cwd,sizeof(cwd));
      cursor = theCWD + strlen(theCWD);
      *cursor = ' ';
      cursor++;
      *cursor = '\0';
    } else
    if ( chdir ( cmdTemp ) < 0) /* Fail case of opening directory */
    {
      printf("ERROR OPENING DIRECTORY:%s \n",cmdTemp);
      return 1;
    } else {
      theCWD = getcwd (cwd,sizeof(cwd));
      cursor = theCWD + strlen(theCWD);
      *cursor = ' ';
      cursor++;
      *cursor = '\0';
    }
  }
  //if the input is for a application (cat/ls) then we need to parse the arguments following it
  else
  if (!strncmp(cmd,"set",3)){
    for(index = 0; tokenArr[1][index] != '\0'; index++) {
      if(tokenArr[1][index] == '\n') {
        tokenArr[1][index] = '\0';
        break;
      }
    }
    char *putEnvStr = malloc(strlen(tokenArr[1]));
    strcpy(putEnvStr,tokenArr[1]);
    if (putenv(putEnvStr) != 0){
      return 1;
    }
  }
  else {
    if ((pid = fork()) < 0){
      printf("fork failed");
      return 1;
    }/* Fail case on fork */
    if (pid == 0){
      int j = 0;//, k = 0, l = 0;
      //int in_index = 0, out_index = 0;
      char input[MAX_INPUT_BUFF_BUFF];
      char output[MAX_INPUT_BUFF_BUFF];
      memset(input, '\0', strlen(input));
      memset(output, '\0', strlen(output));

      /*
      *Check if the redirection operations were used, set flags.
      */
      j = getRedirTarget(cmd, output, '>');
      if(j == 0 ) {
        outRedir = true;
      }
      j = getRedirTarget(cmd, input, '<');
      if(j == 0 ) {
        inRedir = true;
      }
      ///for J search by char for < > | etc.
      // execvp(commands[iterator],tokenArr);

      /**
      *Prior to exec we need to confirm if redirection was used, if so exec in a different manner
      *Using tokens forces us to have spaces between the redirection arguments, this isnt how bash works
      *So parsing character by character, which will have its own issues (file names w/ spaces for example)
      **/
      if(inRedir) {
        //open/close FDs for in redirection
        redirInput(input);
        inRedir = false;
      }
      if(outRedir) {
        //open/close FDs for out redirection
        redirOutput(output);
        outRedir = false;
      }
      if (flag == 1){
        execvp(tokenArgs[0], tokenArgs);
      } else {
        execvp(tokenArr[0],tokenArr);
      }
    } else {
      /* in parent */
      // int status;
      waitpid(pid, NULL /*&status*/, WUNTRACED | WCONTINUED);
    }
  }

  //}
  //}
  // Execute the command, handling built-in commands separately
  // Just echo the command line for now
  //    write(1, cmd, strnlen(cmd, MAX_INPUT_BUFF_BUFF));
  /*******************************************************************
  Print the CWD
  Print the prompt
  ********************************************************************/

  rv = write(1, theCWD, strlen(theCWD));
  rv = write(1, prompt, strlen(prompt));
  memset(cmd,'\0',MAX_INPUT_BUFF_BUFF);
} /* End while */
return 0;
}
