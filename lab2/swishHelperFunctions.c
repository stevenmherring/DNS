#define MAX_INPUT_BUFFER 1024

/*
Helper functions for swish
*/
int getRedirTarget(char *input, char *target, char op);

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
				if(input[j] != ' ') {
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