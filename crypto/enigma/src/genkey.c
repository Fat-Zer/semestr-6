#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "enigma.h"
// #include "cli_utils.h"

#define CRITICAL_ERROR(str,code) do { fprintf(stderr,str); exit(code); } while(0)

#define DEFAULT_KEY_NAME ".engkey"
#define STANDERT_KEY_FILE "$HOME/" DEFAULT_KEY_NAME

void print_usage()
{
	fprintf(stderr, "Usege: genkey [key_file]\n"
			"\tif no key_file named used the default key file(%s)\n"
			"\tif a single hyphen-minus (-) is given as a key_file) stdout is used\n", STANDERT_KEY_FILE);
}

inline FILE* open_key_file(char *kf)
{
//	if(!kf)
//		return 0;
	return fopen(kf,"wb");
}


int main(int argc, char *argv[])
{
	struct enigma eng;
	FILE *fd = 0; 
	
	if(argc==2) {
		if(strcmp("-", argv[1])==0) {
			fd = stdout;
		} else {
			fd = open_key_file(argv[1]);
			if(!fd) {
				printf("error opening key file: \"%s\"\n", argv[1]);
			}
		}
	} else if(argc==1) {
		// set default file name
		char *h = getenv("HOME");
		int h_len = strlen(h);
		char *tmp = malloc(h_len + strlen("DEFAULT_KEY_NAME") + 2);
		strcpy(tmp, h);
		tmp[h_len] = '/';
		strcpy(tmp + h_len + 1, DEFAULT_KEY_NAME);
		fd = open_key_file(tmp);
		if(!fd) {
			printf("error opening key file: \"%s\"\n", tmp);
		}
		free(tmp);
	} else { // argc>2
		print_usage();
		exit(1);
	}
	if(!fd) {
		exit(2);
	}

	if(enigma_gen(&eng)!=0)
		CRITICAL_ERROR("Can't generate enigma key.\n",3);
	if(enigma_fwrite(fd, &eng))
		CRITICAL_ERROR("Can't write key.\n",4);
	
	return 0;
}
