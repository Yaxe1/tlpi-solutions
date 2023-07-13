// Exercise 4-1
// The tee command reads its standard input until end-of-file, writing a copy of 
// the input to standard output and to the file named in its command-line 
// argument. Implement tee using I/O system calls. By default, tee overwrites 
// any existing file with the given name. Implement the -a command-line option 
// (tee -a file), which causes tee to append text to the end of a file if it 
// already exists.

#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#define OPTSTR "ha"
#define USAGE_FMT "Usage: %s [-h] [-a] [FILE]...\n"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

int main(int argc, char* argv[]) {
	// Parse arguments
	bool append_flag = false;
	int opt;
	while ((opt = getopt(argc, argv, OPTSTR)) != -1) {
		switch (opt) {
			case 'a':
				append_flag = true;
				break;
			case 'h':
			default:
				fprintf(stderr, USAGE_FMT, argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	// Open file descriptors to write to
	int open_flags = O_CREAT;
	mode_t file_perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | 
						S_IROTH | S_IWOTH; /* rw-rw-rw */
	
	if (append_flag) {
		open_flags |= O_RDWR | O_APPEND;
	} else {
		open_flags |= O_WRONLY | O_TRUNC;
	}

	int outfc = argc - optind;
	int outfd[outfc];
	for (int i = 0; i < outfc; i++) {
		outfd[i] = open(argv[optind + i], open_flags, file_perms);
	}

	// Read stdin and write to stdout and file descriptors until EOF
	char buffer[BUF_SIZE];
	ssize_t bytes_read;
	do {
		bytes_read = read(STDIN_FILENO, buffer, BUF_SIZE);
		if (bytes_read == -1) {
			errExit("Error reading stdin");
		}
		write(STDOUT_FILENO, buffer, bytes_read);
		for (int i = 0; i < outfc; i++) {
			write(outfd[i], buffer, bytes_read);
		}
	} while (bytes_read > 0);

	// Close file descriptors
	for (int i = 0; i < outfc; i++) {
		close(outfd[i]);
	}
}
