/* CSE 306: Sea Wolves Interactive SHell */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "mysyscall.h"

// Assume no input line will be longer than 1024 bytes
#define MAX_INPUT_BUFF_BUFF 1024

extern char **environ;


int main (int argc, char ** argv, char **envp) {

  int finished = 0;
  char *prompt = "swish> ";
  char cmd[MAX_INPUT_BUFF_BUFF];
  char cmdTemp[MAX_INPUT_BUFF_BUFF];
  char *EXIT_CMD =  "exit\n";
  char cwd[MAX_INPUT_BUFF_BUFF]; 
  /********************************************************************
   Setting the commands that the shell recognizes 
   ********************************************************************/
  char *commandsNL[] = { "ls\n","cd\n","pwd\n"}; 
  char *commands[] = { "ls","cd","pwd"}; 
  

  while (!finished) {
  char *theCWD = getcwd(cwd,sizeof(cwd));

  char *cursor;

  cursor = theCWD + strlen(theCWD);
  *cursor = ' ';
  cursor++;
  *cursor = '\0';
  cursor =NULL ;



  char last_char;
  int rv;
  int count;


  /*******************************************************************
  Print the CWD
  Print the prompt
  ********************************************************************/
  rv = write(1, theCWD, strlen(theCWD));
  rv = write(1, prompt, strlen(prompt));
  int iterator =0;
  int pid=-1;


    for(iterator= 0; iterator < 3; iterator++){
      if (!strncmp(commandsNL[iterator],cmd,2)){
          
          if (strncmp(commandsNL[1],cmd,2) == 0){

          cursor = cmd;
          cursor+=3;
          strcpy(cmdTemp,cursor);

          
          cursor =cmdTemp;
          cursor+=strlen(cmdTemp)-1;
          *cursor = '\0'; 
            if (strcmp(cmd,"cd\n") == 0){
              printf("Needs to be implemented");
              return 1;
            } else 
                    if (chdir(cmdTemp) < 0)
                    {
                      printf("ERROR OPENING DIRECTORY:%s \n",cmdTemp);
                      return 1;

                    } 
                
          }
      if ((pid = fork()) < 0) printf("fork failed");
        if (pid == 0){
          execvp(commands[iterator],argv);
          printf("Executing commands[iterator]");
          
          }

        }
      }
     
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
	  if(strcmp(cmd,EXIT_CMD) == 0) { printf("Exiting. "); return 0; }



  

    // Execute the command, handling built-in commands separately 
    // Just echo the command line for now
    write(1, cmd, strnlen(cmd, MAX_INPUT_BUFF_BUFF));

  } /* End while */

  return 0;
}
