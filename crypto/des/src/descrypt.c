#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "des.h"
#include "cli_utils.h"

//#define STANDERT_KEY_FILE "$HOME/.deskey"
#define CRITICAL_ERROR(code,...) { fprintf(stderr, __VA_ARGS__); exit(code); }

void engcrypt_parse_params(int argc, char **argv);
void print_usage();
void open_inp_file();
void open_outp_file();
void clean();

void parse_key_str();

void do_crypt();

struct engcrypt_shared_data
{
	char* inp_file_name;
	char* outp_file_name;
	char* key_str;
	
	FILE* inp_file;
	FILE* outp_file;
	
	bool usage_flag;
	bool decrypt_flag;
    des_key_t key;
} sd={0,0,0, 0,0, 0,0,0};

/** Поделить main на этапы-функции, наверное, правильно.
 * Это лучше, чем мейн на 200 строк... хоть ради этого 
 * мы и ввели глобальную переменную....
 */
int main(int argc, char **argv)
{
	engcrypt_parse_params(argc, argv);
	if(sd.usage_flag)
	{
		print_usage();
		return 0;
	}
	open_inp_file();
	open_outp_file();
	parse_key_str();
	
	do_crypt();
	
	clean();
		
	return 0;
}

void engcrypt_parse_params(int argc, char **argv)
{
	struct param_descr pd[]={	
		{'k',	"key",	{PA_GET_STRING, &sd.key_str}},
		{'u',	"usage",{PA_SET_FLAG,	&sd.usage_flag}},
		{'d',   "decrypt",{PA_SET_FLAG,   &sd.decrypt_flag}},
        {'c',   "crypt",{PA_RESET_FLAG,   &sd.decrypt_flag}},
        {0,		0,		{0,0}}	};
	struct param_action def_pa[]={
		{PA_GET_STRING, &sd.inp_file_name},
		{PA_GET_STRING, &sd.outp_file_name},
		{0,	0}};
	int parsed;

	parsed=parse_params(argv, pd, def_pa);
	if(parsed != argc)
	{
		print_usage();
		CRITICAL_ERROR(1,"Wrong parametres\n");
	}
}

void print_usage()
{
	fprintf(stderr, "Usege: descrypt [-k key] input_file output_file\n"
			"\t-k\t--key key\t specify the 56-bit key in hex form HH-HH-HH-HH-HH-HH-HH\n"
			"\t-u\t--usage\t print this usage message\n"
			"\tif a single hyphen-minus (-) is given as a file stdout(stdin) is used\n");
}

void open_inp_file()
{
	if(!sd.inp_file_name)
		CRITICAL_ERROR(2,"Input file not specified \n")
	else if(strcmp(sd.inp_file_name,"-")==0)
		sd.inp_file=stdin;
	else
	{
		sd.inp_file = fopen(sd.inp_file_name,"rb");
		if(!sd.inp_file)
			CRITICAL_ERROR(2,"Can't open input file \"%s\"\n",
							sd.inp_file_name);
	}
}

void open_outp_file()
{
	if(!sd.outp_file_name)
		CRITICAL_ERROR(2,"Input file not specified \n")
	else if(strcmp(sd.outp_file_name,"-")==0)
		sd.outp_file=stdout;
	else
	{
		sd.outp_file = fopen(sd.outp_file_name,"wb");
		if(!sd.outp_file)
			CRITICAL_ERROR(2,"Can't open input file \"%s\"\n",
							sd.outp_file_name);
	}
}

void parse_key_str()
{
	size_t i;
	unsigned buff=0;
	int parsed;
    char *key_str = sd.key_str;
	if(!key_str)
		CRITICAL_ERROR(3, "No key specified.\n");

	parsed = sscanf(key_str,"%4x",&buff);
	if(!parsed)
		CRITICAL_ERROR(3, "Bad key specified: \"%s\".\n",
						sd.key_str);
	sd.key = buff;
    key_str+=4;
	for(i=1;i<4;i++)
	{
		parsed = sscanf(key_str,"-%4x",&buff);
		if(!parsed)
			CRITICAL_ERROR(3, "Bad key specified: \"%s\".\n",
							sd.key_str);
		sd.key <<= 16;
		sd.key |= buff;
        key_str+=5;
	}
	sd.key = reverse_bits64(sd.key);
}

void do_crypt()
{
    int err;
    err = des_fcrypt(sd.outp_file, sd.inp_file, sd.key, sd.decrypt_flag ? DES_DECRYPT_ACT : DES_ENCRYPT_ACT);
    if(err)
        CRITICAL_ERROR(4, "%s", des_err_string(err));
}

void clean()
{
	fclose(sd.inp_file);
	fclose(sd.outp_file);
	if(sd.key_str) free(sd.key_str);
	if(sd.inp_file_name) free(sd.inp_file_name);
	if(sd.outp_file_name) free(sd.outp_file_name);
}


/*
#define CRITICAL_ERROR(str,code) { fprintf(stderr,str); exit(code); }

enum ERR_NO_KEY{};



void print_usege()
{
	fprintf(stderr, "Usege: engcrypt [-k key_file] [in_file [out_file]]\n"
			"if no key_file named ~/.enigma_key is used.\n"
			"if no in_file named (or it's a '-') stdin is used.\n"
			"if no out_file named (or it's a '-') stdout is used.\n");

}

FILE* open_key_file(char *kf)
{
	if(!kf)
		return 0;
	return fopen(kf,"wb");
}

int main(int argc, char **argv)
{
	FILE *ifd=stdin; 
	FILE *ofd=stdout; 
	
	struct enigma eng;
	
	if(argc>1)
	{
		if(argc>2)
		{
			print_usege();
			return 1;
		}
		if(strcmp(argv[1],"-")!=0)
			if(!(fd=open_key_file(argv[1])))
				CRITICAL_ERROR("Can't open key file.\n",2)
	}
	if(enigma_gen(&eng))
		CRITICAL_ERROR("Can't generate enigma.\n",3)
	if(enigma_fwrite(fd, &eng))
		CRITICAL_ERROR("Can't write key.\n",4)
	
	return 0;

}

*/
