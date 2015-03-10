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
  char cmdTemp[MAX_INPUT_BUFF_BUFF];
  char *EXIT_CMD =  "exit\n";
  char cwd[MAX_INPUT_BUFF_BUFF];

  /********************************************************************
  Setting the commands that the shell recognizes
  ********************************************************************/
  char *commandsNL[] = { "ls\n","cd\n","pwd\n","printenv\n","putenv\n"};
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

  strcpy(tokenBuff,cmd);
  //printf("TokenBuff pre Tokenizer : %s \n",tokenBuff);
  char *tokenArr[50];
  int counter = 0;
  char* token = strtok(tokenBuff, " ");
  while (token) {
    tokenArr[counter++] = token;
    token = strtok(NULL, " ");
  }
  tokenArr[counter] = NULL;

  int index = 0;
  for(index = 0; tokenArr[counter - 1][index] != '\0'; index++) {
    if(tokenArr[counter - 1][index] == '\n') {
      // printf("Token: %s",tokenArr[counter - 1]);
      tokenArr[counter - 1][index] = '\0';
      break;
    }
  }
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
  if (strncmp(commandsNL[1],cmd,2) == 0){
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
  if (!strncmp(cmd,"putenv",6)){
    printf("Attempting putenv. %s ",tokenArr[1]);
    putenv(tokenArr[1]);
  }
  if ((pid = fork()) < 0){
    printf("fork failed");
    return 1;
  }/* Fail case on fork */
  if (pid == 0){
    int j = 0, k = 0;
    char input[MAX_INPUT_BUFF_BUFF];
    char output[MAX_INPUT_BUFF_BUFF];
    memset(input, '\0', strlen(input));
    memset(output, '\0', strlen(output));
    printf("cmd: %s", cmd);
    for(j = 0; j < (strlen(cmd) - 1); j++) {
      /**
      * Search for <, >, or |
      **/
      /*
      Handle input redirection
      Check character of the input for <
      If found, set a cursor to that position + 1
      if space cursor + 1
      store next argument from input
      */
      printf("before input: %c\n", cmd[j]);
      if(cmd[j] == '<') {
        inRedir = true;
        int in_index;
        int l = 0;
        if(cmd[j+1] == ' ') {
          in_index = 2;
        }
        else {
          in_index = 1;
        }
        for(k = j+in_index; k < strlen(cmd); k++) {
          if(cmd[k] != ' ') {
            input[l] = cmd[k];
            l++;
          }
          else {
            break;
          }
        }
        input[l+1] = '\0';
      } //assign input buffer
      /*
      Handle output redirection
      Check character of the input for >
      If found, set cursor at index + 1
      if cursor = ' ' cursor =+ 1
      store next argument
      */
      else if(cmd[j] == '>') {
        outRedir = true;
        int out_index = 0;
        int l = 0;
        if(cmd[j+1] == ' ') {
          out_index = 1;
        }
        else{
          out_index = 0;
        }
        for(k = j+out_index; k < strlen(cmd); k++) {
          if(cmd[k] != ' ' || cmd[k] != '\n') {
            output[l] = cmd[k];
            l++;
          }
          else {
            break;
          }
        }
        output[l+1] = '\0';
      } //assign output buffer}
      else if(cmd[j] == '|') {
        //piping potentially won't even be here.
      } //piping
    }//for J search by char for < > | etc.
    //printf("Input: %s\n", input);
    //printf("Output: %s\n", output);
    // execvp(commands[iterator],tokenArr);
    //printf("TokenArr : %s ",tokenArr[0]);
    // printf("TokenArr : %s ",tokenArr[1]);

    /**
    *Prior to exec we need to confirm if redirection was used, if so exec in a different manner
    *Using tokens forces us to have spaces between the redirection arguments, this isnt how bash works
    *So parsing character by character, which will have its own issues (file names w/ spaces for example)
    **/
    printf("output: %s", output);
    if(inRedir) {
      //open/close FDs for in redirection
      FILE *file_in = fopen(input, O_RDONLY);
      int fd_in = file_in->_fileno;
      dup2(fd_in, STDIN_FILENO);
      close(fd_in);
      inRedir = false;
    }
    if(outRedir) {
      //open/close FDs for out redirection
      FILE *file_out = fopen(output, "ab+");
      int fd_out = file_out->_fileno;
      dup2(fd_out, STDOUT_FILENO);
      close(fd_out);
      outRedir = false;
    }
    execvp(tokenArr[0], tokenArr);
    //cursor = cmd;
    //*cursor = '\n';
  } else {
    /* in parent */
    // int status;
    waitpid(pid, NULL /*&status*/, WUNTRACED | WCONTINUED);
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
  if(!strncmp(cmd,EXIT_CMD,4)) {
    finished = 1;
    break;
    return 0;
  }
  rv = write(1, theCWD, strlen(theCWD));
  rv = write(1, prompt, strlen(prompt));
  memset(cmd,'\0',MAX_INPUT_BUFF_BUFF);
} /* End while */
return 0;
}
