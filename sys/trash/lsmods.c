#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stropts.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
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
		int sz, i;
		struct str_list lst;
		sz = ioctl(fd, I_LIST, 0);
		if (sz == -1) {
			fprintf(stderr, "ioctl failed. reason: %s", strerror(errno));
			exit(EXIT_FAILURE);
		}
		lst.sl_nmods = sz;
		lst.sl_modlist = malloc((sz+1) * sizeof(struct str_mlist));
		if( !lst.sl_modlist ) {
			fprintf(stderr, "malloc failed. reason: %s", strerror(errno));
			exit(EXIT_FAILURE);
		}
		if( ioctl(fd, I_LIST, &lst) == -1 ) {
			fprintf(stderr, "ioctl failed. reason: %s", strerror(errno));
			free(lst.sl_modlist);
			exit(EXIT_FAILURE);
		}
		for(i=0; i<sz; i++) {
			printf("module: %s", lst.sl_modlist[i].l_name);
		}
		printf("driver: %s", lst.sl_modlist[sz].l_name);

	} else {
		printf("device is not a stream device\n");
	}

	close(fd);
	return 0;
}
