#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "enigma.h"
#include "cli_utils.h"

#define CRITICAL_ERROR(str,code) { fprintf(stderr,str); exit(code); }
#define STANDERT_KEY_FILE "$HOME/.engkey"

void print_usege()
{
	fprintf(stderr, "Usege: genkey [key_file]\n"
			"\tif no key_file named used default key file("STANDERT_KEY_FILE")\n"
			"\tif a single hyphen-minus (-) is given as a key_file) stdout is used\n");
}

FILE* open_key_file(char *kf)
{
//	if(!kf)
//		return 0;
	return fopen(kf,"wb");
}

int main(int argc, char *argv[])
{
	struct enigma eng;
	FILE *fd=stdout; 
	char *key_fname=0;
	struct param_descr param_descr[] = {
		{'k',	"key",	{PA_GET_STRING,&key_fname}},
		{0,		0} };
	struct param_action default_acts[] = {
		{0,		0} };
	int tmp;

	char* def_file = malloc(var_subst_len(STANDERT_KEY_FILE));
	var_subst(def_file,STANDERT_KEY_FILE,0);

	tmp = parse_params(argv, param_descr, default_acts);
	if(tmp!=argc)
	{
		fprintf(stderr, "error in parameter %d: \"%s\"\n", tmp, argv[tmp]);
		
		print_usege();
		return 1;
	}
	if(!key_fname) {
		if(!(fd=open_key_file(STANDERT_KEY_FILE)))
			CRITICAL_ERROR("Can't open key file.\n",2)
	} else if(strcmp(key_fname,"-")!=0)
		if(!(fd=open_key_file(key_fname)))
			CRITICAL_ERROR("Can't open key file.\n",2)

	if(enigma_gen(&eng))
		CRITICAL_ERROR("Can't generate enigma key.\n",3)
	if(enigma_fwrite(fd, &eng))
		CRITICAL_ERROR("Can't write key.\n",4)
	
	return 0;

}


