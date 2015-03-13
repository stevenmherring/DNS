#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#define MAX_INPUT_BUFFER 1024

/*
Helper functions for swish
*/
int getRedirTarget(char *input, char *target, char op);
char** strsplit(char* str, const char d);
void removeSpaces(char* input, char **ret, int index);
int findFlagD (char ** argv, int argc);

int getArgs(char ** argv, int argc){
	int i =0;
	for (i =0; i<argc; i++){

	printf("Argv[%d] : %s \n",i,argv[i]);
	}
	return 0;
}


/**
*getRedirTarget takes a string and a character, returns the preceeding string
*works if the string is connected to the character or separated by a space
*returns 0 if successfully found the operand and string, else 1
**/
int getRedirTarget(char *input, char *target, char op) {
	int i = 0, j = 0, k = 0;
	int index = 0;
	int found = 1;
	for(i = 0; i < (strlen(input) - 1); i++) {
		/**
		*Search for target
		**/
		if(input[i] == op) {
			found = 0;
			if(input[i+1] == ' ') {
				index = 2;
			} else {
				index = 1;
			}
			for(j = i+index; j < (strlen(input) - 1); j++) {
				if(input[j] != ' ' && input[j] != '<' && input[j] != '>') {
					target[k] = input[j];
					k++;
				} else {
					break;
				}
			}
			target[k] = '\0';
			break;
		}//if op found
		else {
		}
	}
	return found;
}

/**
*strsplit splits a string up and feeds it back as a 2d array
**/
char** strsplit(char* str, const char d) {
	char** r = 0;
	int count = 0;
	char* temp = str;
	char* ld = 0;

	while(*temp)
	{
		if(d == *temp)
		{
			count++;
			ld = temp;
		}
		temp++;
	}
	count += ld < (str + strlen(str) - 1);
	count++;
	r = malloc(sizeof(char*) * count);
	if(r){
		int index = 0;
		char* token = strtok(str, &d);
		while(token) {
			*(r + index++) = strdup(token);
			token = strtok(0, &d);
		}
		*(r + index) = 0;
	}
	return r;
}

void removeSpaces(char* input, char **ret, int index){
	int rCount = 0;
	char *rToke = strtok(input, " ");
	while(rToke) {
		ret[rCount++] = rToke;
		rToke = strtok(NULL, " ");
	}
	//return ret;
	//char *r = input;
	//if(input[0] == ' ') {
	//	for(i = 0, j = 1; i < strlen(input); i++, j++) {
	//		r[i] = input[j];
	//	}
	//}




}

int printWolfie(){
  char wolfieBuffer[255];
  char *colors[] = {"\x1B[0m","\x1B[31m","\x1B[32m","\x1B[33m","\x1B[34m","\x1B[35m","\x1B[36m","\x1B[37m"};
  FILE * fd;
  if ((fd = fopen("catWolfieSAFE.txt","r")) < 0){
    printf("Error opening");
    return 1;
  }
  //fgets(wolfieBuffer,125,fd);
  int index =1;
  int iterator =0;
  int colorCounter =0;
  srand(time(NULL));
  while(index != 0){
    for (iterator =1;iterator < 24;iterator++){
      
    printf("\n");
    }
    for (iterator =1;iterator < 24;iterator++){
    colorCounter = rand() % 8;
    index = (int) fgets(wolfieBuffer,125,fd);
    if(index!=0){
      
    printf("%s%s",colors[colorCounter] ,wolfieBuffer);
    }
    }
    
    sleep(1);
  }
    return 0;
}

/*
char* getCMDargs(char* command,char* bufferToParse){
	int i =0;
	char *cmdArgs[MAX_INPUT_BUFFER];
	strcpy(cmdArgs,bufferToParse);
	char *cursor = cmdArgs;
	for (i=0; i<strlen(bufferToParse);i++){
		if (*cursor == ' '){
			cursor++;
			printf("%s",cursor);
		}
		cursor+=i;
	}
}

int parseExecFlags(char** commands,int commandsNLchoice, char ** argv, int argc){

	//getCMDargs();

    //execvp(commands[iterator],argv);
	return 0;
} */

/* Process a ptr to an array of ptrs, and return 0 if -d is flag, 1 if it is not

int findFlagD (char ** argv, int argc){

	char* cursor;
	int iterateArgv = 0;
	for (iterateArgv = 1; iterateArgv < argc; iterateArgv++){
	
	cursor = argv[iterateArgv];
	while (cursor != NULL) {
		if (!strcmp (cursor , "-d")){
			return 0;
		} else if (strlen (cursor) > 1 ){
				cursor += strle(
				if( strncmp ( cursor, ".sh", 3)){
				
				}
			}
		cursor++;
	}
	}
	return 1;
}

 Takes a buffer that contains the file for the script, a char* contains the arguments for the script and return whether the execution succeeded 

int execScript (char *scriptName, char *scriptArgs[]){

	int returnValScript;
	char *const currentEnv = NULL;
	currentEnv = getenv("PATH");
	if ( (returnValScript = execvpe(scriptName, scriptArgs, currentEnv) ) == -1){
		printf("Error executing script");	
		return -1;
	}

}
*/
