#define MAX_INPUT_BUFFER 1024

/*
Helper functions for swish
*/

int getArgs(char ** argv, int argc){
	int i =0;
	for (i =0; i<argc; i++){

	printf("Argv[%d] : %s \n",i,argv[i]);
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