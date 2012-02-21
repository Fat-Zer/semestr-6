#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "enigma/enigma.h"

#define CRITICAL_ERROR(str,code) { fprintf(stderr,str); exit(code); }

void print_usege()
{
	fprintf(stderr, "Usege: genkey [key_file]\n"
			"if no key_file named (or if a single hyphen-minus (-)"
				" is given as a key_file) stdout is used\n");
}

FILE* open_key_file(char *kf)
{
	if(!kf)
		return 0;
	return fopen(kf,"w");
}

int main(int argc, char **argv)
{
	FILE *fd=stdout; 

	if(argc>1)
	{
		if(!strstr(argv[1],"-"))
			if(!(fd=open_key_file(argv[1])))
				CRITICAL_ERROR("Can't open key file.\n",1)
		if(argc>2)
		{
			print_usege();
			exit(2);
		}
	}


}


