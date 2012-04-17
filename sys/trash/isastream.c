#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stropts.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>

int main(int argc, char **argv) {
	int fd, rc;

	if(argc != 2) {
		fprintf(stderr, "Usage: %s <dev_file>", argv[0]);
		exit(EXIT_FAILURE);
	}

	fd = open(argv[1], O_RDONLY);
	if(fd == -1) {
		fprintf(stderr, "failed to open file %s reason: %s", argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}

	rc = isastream(fd);
	if( rc == 0 ) {
		fprintf(stderr, "device is not a stream device\n");
	} else {
		fprintf(stderr, "device is a stream device\n");
	}

	return 0;
}
