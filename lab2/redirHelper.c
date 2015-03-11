void redirInput(char *input);
void redirOutput(char *input);

/**
*redirInput small method for handling input redirection
**/
void redirInput(char *input) {
	FILE *fd = fopen(input, O_RDONLY);
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