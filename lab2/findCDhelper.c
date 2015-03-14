/*
Check the command line for the CD command and handles
Return value = 0 success
Return value = 1 failure
*/
#define MAX_INPUT_BUFF_BUFF 1024
int checkForCd (char *cmd);
int checkForCd (char *cmd){
  char *cursor = NULL;

  char cwd[MAX_INPUT_BUFF_BUFF];
  char cmdTemp[MAX_INPUT_BUFF_BUFF];

  char *theCWD = getcwd(cwd,sizeof(cwd));
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
  } else if ( chdir ( cmdTemp ) < 0){
          printf("ERROR OPENING DIRECTORY:%s \n",cmdTemp);
          return 1;
          } else {
                    theCWD = getcwd (cwd,sizeof(cwd));
                    cursor = theCWD + strlen(theCWD);
                    *cursor = ' ';
                    cursor++;
                    *cursor = '\0';
                    printf("Success changing.. New CWD %s \n",theCWD);
                  }

  return 0;
  } else return 1;

}

int checkForExit (char *cmd){
  char *EXIT_CMD =  "exit\n";
  if(!strncmp(cmd,EXIT_CMD,4)) {
    exit(0);
    return 0;
  }
  return 1;
}

void removeNewLinesLastElementArray(char **arrayToNull, int numOfElementsInArr){

  // Null terminate the tokenArr array.
  int index = 0;
  for(index = 0; arrayToNull[numOfElementsInArr - 1][index] != '\0'; index++) {
    if(arrayToNull[numOfElementsInArr - 1][index] == '\n') {
      arrayToNull[numOfElementsInArr - 1][index] = '\0';
      break;
    }
  }

  arrayToNull[numOfElementsInArr] = NULL;
}
int checkForSet (char *cmd){
  char *tokenArr[50];
  char *tokenArgs[50];
  char tokenBuff[MAX_INPUT_BUFF_BUFF];
  int pid=-1;
  int counter = 0;
  int counterArgs =0;
  int flag =0;
  strcpy(tokenBuff,cmd); // Token buff now holds the entire command line
  char* token = strtok(tokenBuff, " "); // Token holds the first space delimited command line

  tokenArgs[counterArgs++] = token; // Token Args now holds the first executable command, counter for this array incremented
  while (token) { // While the command line has more space delimited strings
    if (!strncmp(token,"$",1)){ // If the token string starts with a $ variable
      flag = 1; // We will set the flag to use execvp with the argument dependent exec
      char *nlRemove = token; // Make a cursor to this tokenized string
      token++; // Move the pointer of the tokenized string to the immediate value after '$' ie , if $PATH then token == 'PATH\n'
      nlRemove+=strlen(token); // Move the cursor to the end of the Token
      *nlRemove = '\0'; // Remove the newline from the 'PATH\n' example
      if((tokenArgs[counterArgs++] = getenv(token)) == NULL){  // Get the environmental variable associated with this new Token
        printf("Cannot find that environment variable.");
        exit(1);
      }
    }//End if '$xxx' .. Environment variable value now stored in tokenArgs[]
    if (!strncmp(token,"-",1)){ // Some argument found, place this in tokenArg[]
      flag = 1; // Use the tokenArg[] exec
      tokenArgs[counterArgs++] = token; // Place the tokenized string into the tokenArg[]
    } else
    if (!strncmp(token,">",1) || !strncmp(token, "<",1) || !strncmp(token, "|", 1)){ // Deprecated IGNORES these 3 chars and does not place anything
      if(strlen(token) == 1){
        token = strtok(NULL, " ");
      } //if char* length = 1
      else {
      } //else char* length > 1
    }//if first char = >
    else { // If these cases are not found, it will assume the value is a modifier to another call and places this value in token Arr.
      tokenArr[counter++] = token;
    } // else
    token = strtok(NULL, " ");
  }//while
  removeNewLinesLastElementArray(tokenArr,counter);
  removeNewLinesLastElementArray(tokenArgs,counterArgs);
  if (!strncmp(cmd,"set",3)){
    char *putEnvStr = malloc(strlen(tokenArr[1]));
    strcpy(putEnvStr,tokenArr[1]);
    if (putenv(putEnvStr) != 0){
      return 1;
      printf("Could not put environment variable");
      exit(1);
    }
  }
    if ((pid = fork()) < 0){
      printf("fork failed");
      return 1;
    }/* Fail case on fork */
    if (pid == 0){
      if (flag == 1){
        execvp(tokenArgs[0], tokenArgs);
        return 0;

      } else
        execvp(tokenArr[0],tokenArr);
        return 0;

    } else {
      /* in parent */
      // int status;
      waitpid(pid, NULL /*&status*/, WUNTRACED | WCONTINUED);
    }
  return 0;
}
