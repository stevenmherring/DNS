void redirControl(char* buffer);
void removeQ(char* str);

typedef enum {false, true} bool; //boolean enumerations

/*remove control is for removing qoutes*/
void removeQ(char* str) {
	char *ps;
	for (ps = str; *ps != '\0'; ps++) {
		*ps = *(ps+1);
	}
	str[strlen(str) - 1] = '\0';
}

/* redirControl is in charge of checking the buffer for redirection flags and processing them */

void redirControl(char *buffer){
	int MAX_PIPES = 10, MAX_ARGUMENTS = 10;
	char *temp = NULL, *pipeCommands[MAX_PIPES], *cmdArgs[MAX_ARGUMENTS];
	int currPipe[2]; // since we're doing this iteratively, need to keep track of all pipes
	int oldPipe[2];
	int pipesCount = -1;
	int count, i, status;
	char input[50], output[50];
	bool checkOut, checkIn;
	pid_t pidc;


	do {
		temp = strsep(&buffer, "|");
		if(temp != NULL) {
			if(strlen(temp) > 0){
				pipeCommands[++pipesCount] = temp;
			}
		}
	} while(temp); //process the buffer and see how many pipes we're dealing with.
	cmdArgs[++pipesCount] = NULL;
	for(i = 0; i < pipesCount; i++) { //run through each pipe command
		count = -1;
		do {
			temp = strsep(&pipeCommands[i], " ");
			if(temp != NULL) {
				if(strlen(temp) > 0) {
					//if this is the first pipe parameter or the last check for redirection
					//is temp = 1? is it <? then get just process that and move temp forward
					if(i == 0 && i == pipesCount-1) { // no pipes so we can accept a change in input
						if(strlen(temp) > 1 && getRedirTarget(temp, input, '<') ==  0) {
							checkIn = true;
							cmdArgs[count] = NULL;
						}
						if(strlen(temp) > 1 && getRedirTarget(temp, output, '>') ==  0) {
							//set a flag for redirection, and redirect during FORK process?
							strcpy(temp, output);
							checkOut = true;
							cmdArgs[count] = NULL;
						}
						if(strlen(temp) == 1 && (temp[i] == '<' || temp[i] == '>')) {
							if(temp[i] == '<') {
								temp = strsep(&pipeCommands[i], " ");
								checkIn = true;
								strcpy(input, temp);
							} else if(temp[i] == '>') {
								temp = strsep(&pipeCommands[i], " ");
								strcpy(output, temp);
								checkOut = true;
							}
							temp = strsep(&pipeCommands[i], " ");
						}
					}//no pipes check
					else if(i == 0 && strlen(temp) > 1) { // first pipe
						if(getRedirTarget(temp, input, '<') ==  0) {
							checkIn = true;
							cmdArgs[count] = NULL;
							if(strlen(temp) == 1) {
								temp = strsep(&pipeCommands[i], " ");
							}
						}
					}
					else if(i == pipesCount-1) { // last pipe
						if(strlen(temp) > 1){
							if(getRedirTarget(temp, output, '>') == 0) {
								checkOut = true;
								temp = NULL;
							}
						}
						else if(strlen(temp) == 1) {
								temp = strsep(&pipeCommands[i], " ");
								strcpy(output, temp);
								checkOut = true;
								temp = NULL;
							}
					}
					if(temp != NULL) {
						if(!strcmp(temp, "~")) {
							strcpy(temp, "/home/user");
						} // if pass ~ replace with the default directory
						if(((temp[0] == '\"') && (temp[strlen(temp) - 1] == '\"')) || ((temp[0] == '\'') && (temp[strlen(temp) - 1] =='\''))) {
							char *ps;
							for (ps = temp; *ps != '\0'; ps++) {
								*ps = *(ps+1);
							}
							temp[strlen(temp) - 1] = '\0';
						}
					}//if there is ' ' or " " surrounding an argument parameter, remove them.
					cmdArgs[++count] = temp;
				}
			}
		} while(temp);
		cmdArgs[++count] = NULL;
		//execute commands
		if(i < pipesCount-1) {
			pipe(currPipe);
		}
		//redirInput(input);
		pidc = fork(); //attempt new child process
		if(pidc == 0) { //if we're in child
			if(checkIn){
				redirInput(input);
			}
			if(checkOut){
				redirOutput(output);
			}
			if(i > 0) { //process command
				close(oldPipe[1]);
				dup2(oldPipe[0], 0);
				close(oldPipe[0]);
			} //is there a previous command
			if(i < pipesCount-1) {
				close(currPipe[0]);
				dup2(currPipe[1], 1);
				close(currPipe[1]);
			} //are there commands left to execte
		if(execvp(cmdArgs[0], cmdArgs) == -1) {
				printf("Command did not execute, check command: %s\n", cmdArgs[0]);
			}
			exit(1);
		}
		else { //not in the child
			if(i > 0) {
				close(oldPipe[0]);
				close(oldPipe[1]);
			} //previous command?
			if(i < pipesCount-1) {
				oldPipe[0] = currPipe[0];
				oldPipe[1] = currPipe[1];
			} //is there another command to ezecute?
			if(i == pipesCount-1) {
				waitpid(pidc, &status, 0);
			}
		}
	}
} //end of redirControl
