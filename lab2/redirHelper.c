void redirInput(char *input);
void redirOutput(char *input);
void recursivePipe(char *pipes[], int index, int fd_in);
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
		if((pipe(fd) == -1) || ((cpid = fork()) -- -1)) {
			//error handle, bad fork
		}
		if(cpid == 0) {
			isChild = 1;
			close(fd[0]);
			if(dup2(fd_in), STDIN_FILENO) == -1) //error
			if(dup2(fd[1], STDOUT_FILENO) == -1) //error
			else if(close(fd[1]) == -1) //error
			else {
				execlp(pipes[index], pipes[index], NULL);
			}
			close(fd[1]);
			close(fd_in);
			pipeline(pipes,index+1, fd[0]);
		}
	}
}//recursivepipes
