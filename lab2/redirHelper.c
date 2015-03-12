void redirInput(char *input);
void redirOutput(char *input);
void recursivePipe(char *pipes[], int index, int fd_in);
void execute_only_pipes(char* buffer);
/**
*redirInput small method for handling input redirection
**/
void redirInput(char *input) {
	FILE *fd = fopen(input, "ab+");
	int fd_in = fd->_fileno;
	dup2(fd_in, STDIN_FILENO);
	close(fd_in);
}

/**
*redirInput small method for handling input redirection
**/
void redirOutput(char *input) {
	FILE *fd = fopen(input, "ab+");
	int fd_in = fd->_fileno;
	dup2(fd_in, STDOUT_FILENO);
	close(fd_in);
}

/**
*recursivePipe method uhhhh...i wrote it by hand so no comments
**/
void recursivePipe(char *pipes[], int index, int fd_in) {
	write(1, "in pipes", 8);
	if(pipes[index+1] == NULL) {
		if(fd_in != STDIN_FILENO) {
			if(dup2(fd_in, STDIN_FILENO) != -1) {
				close(fd_in);
			} else {
			//space for error handling
			}
		}
		execlp(pipes[index], pipes[index], NULL);
	} else {
		int fd[2];
		pid_t cpid;
		if((pipe(fd) == -1) || ((cpid = fork()) == -1)) {
			//error handle, bad fork
		}
		if(cpid == 0) {
			close(fd[0]);
			if(dup2(fd_in, STDIN_FILENO) == -1) { } //error
			if(dup2(fd[1], STDOUT_FILENO) == -1){ } //error
			else if(close(fd[1]) == -1) { } //error
			else {
				execlp(pipes[index], pipes[index], NULL);
			}
			close(fd[1]);
			close(fd_in);
			recursivePipe(pipes,index+1, fd[0]);
		}
	}
}//recursivepipes

/* Executes the command 'buffer' assuming that doesn't contain redirections */
void execute_only_pipes(char* buffer)
{
		int MAX_PIPES = 10, MAX_ARGUMENTS = 10;
    char *temp = NULL, *pipeCommands[MAX_PIPES], *cmdArgs[MAX_ARGUMENTS];
    int newPipe[2], oldPipe[2], pipesCount, aCount, i, status;
    pid_t pid;

    pipesCount = -1; /* This variable will contain how many pipes the command contains */

    /* Counting the number of pipes and splitting them into pipeCommands */
    do
    {
        temp = strsep(&buffer, "|");

        if(temp != NULL)
        {
            if(strlen(temp) > 0)
            {
                pipeCommands[++pipesCount] = temp;
            }
        }
    } while(temp);

    cmdArgs[++pipesCount] = NULL;

    for(i = 0; i < pipesCount; i++) /* For each command */
    {
        aCount = -1;

        /* Parsing command & arguments */
        do
        {
            temp = strsep(&pipeCommands[i], " ");
            if(temp != NULL)
            {
                if(strlen(temp) > 0)
                {
                    /* If a parameter is ~, then replace it by /home/user */
                    if (!strcmp(temp, "~"))
                        strcpy(temp, "/home/user");
                    cmdArgs[++aCount] = temp;
                }
            }
        } while(temp);
				cmdArgs[++aCount] = NULL;

        /* If there still are commands to be executed */
        if(i < pipesCount-1)
        {
            pipe(newPipe); /* just create a pipe */
        }

        pid = fork();

        if(pid == 0)  /* Child */
        {
            /* If there is a previous command */
            if(i > 0)
            {
                close(oldPipe[1]);
                dup2(oldPipe[0], 0);
                close(oldPipe[0]);
            }

            /* If there still are commands to be executed */
            if(i < pipesCount-1)
            {
                close(newPipe[0]);
                dup2(newPipe[1], 1);
                close(newPipe[1]);
            }


            if (execvp(cmdArgs[0], cmdArgs) == -1)
            {
                printf("Error. Command not found: %s\n", cmdArgs[0]);
            }
            exit(1);
        }
        else /* Father */
        {
            /* If there is a previous command */
            if(i > 0)
            {
                close(oldPipe[0]);
                close(oldPipe[1]);
            }

            /* do we have a next command? */
            if(i < pipesCount-1)
            {
                oldPipe[0] = newPipe[0];
                oldPipe[1] = newPipe[1];
            }

            /* wait for last command process? */
            if(i == pipesCount-1)
            {
                waitpid(pid, &status, 0);
            }
        }
    }
}
