#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "enigma.h"
#include "cli_utils.h"

#define DEFAULT_KEY_NAME ".engkey"
#define STANDERT_KEY_FILE "$HOME/" DEFAULT_KEY_NAME
#define CRITICAL_ERROR(code,...) { fprintf(stderr, __VA_ARGS__); exit(code); }

void engcrypt_parse_params(int argc, char **argv);
void print_usage();
void open_key_file();
void open_default_key_file();
void open_inp_file();
void open_outp_file();
void clean();

void parse_init_rotor_str();

void crypt();
void do_crypt();

/** Начхать мне, что глобальные переменные-зло
 * без них все функции выглядят невероятно убогими...
 * всюду из мейна передавать один параметр - идиотизм
 * и меня это задолбало! Пусть лучше будет одна глабальная
 * структура, чем это уродство!
 *
 * Да, и критические ошибки каждая функция будет 
 * выводить сама и убивать при этом программу.
 * Цена за "правильный" код слишком высока - уродство
 */
struct engcrypt_shared_data
{
	char* key_file_name;
	char* inp_file_name;
	char* outp_file_name;
	char* init_rotor_str;
	
	FILE* key_file;
	FILE* inp_file;
	FILE* outp_file;

	char usage_flag;
	int init_vals[3];

	struct enigma eng;
} sd={0,0,0,0, 0,0,0, 0,{0,0,0}};

/** Поделить main на этапы-функции, наверное, правильно.
 * Это лучше, чем мейн на 200 строк... хоть ради этого 
 * мы и ввели глобальную переменную....
 * 			-- я до отчисления
 */

/** не... всё же лучше 
 * 			-- я после востановления
 */

int main(int argc, char **argv)
{
	engcrypt_parse_params(argc, argv);
	if(sd.usage_flag)
	{
		print_usage();
		return 0;
	}
	open_key_file();
	open_inp_file();
	open_outp_file();
	if(sd.init_rotor_str)
		parse_init_rotor_str();
	crypt();
	
	clean();
		
	return 0;
}

void engcrypt_parse_params(int argc, char **argv)
{
	struct param_descr pd[]={	
		{'k',	"key",	{PA_GET_STRING, &sd.key_file_name}},
		{'i',	"init",	{PA_GET_STRING, &sd.init_rotor_str}},
		{'u',	"usage",{PA_SET_FLAG,	&sd.usage_flag}},
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
	fprintf(stderr, "Usege: enigma [-k key_file] [-i initial_values] input_file output_file\n"
			"\t-k\t--key key_file\t specify the key file used\n"
			"\t\t\t\tif no key_file named used default key file("STANDERT_KEY_FILE")\n"
			"\t-i\t--init initial_values\t specify the initial values for roters\n"
			"\t\t\t\t in form %%d;%%d;%%d (e.g. 2;132;17)\n"
			"\t-u\t--usage\t print this usage message\n"
			"\tif a single hyphen-minus (-) is given as a file) stdout(stdin) is used\n");
}

void open_key_file()
{
	if(!sd.key_file_name)
		open_default_key_file();
	else if(strcmp(sd.key_file_name,"-")==0)
		sd.key_file=stdin;
	else
	{
		sd.key_file = fopen(sd.key_file_name,"rb");
		if(!sd.key_file)
			CRITICAL_ERROR(2,"Can't open key file \"%s\"\n",
							sd.key_file_name);
	}
}

void open_default_key_file()
{
			// set default file name
	char *h = getenv("HOME");
	int h_len = strlen(h);
	char *tmp = malloc(h_len + strlen("DEFAULT_KEY_NAME") + 2);
	strcpy(tmp, h);
	tmp[h_len] = '/';
	strcpy(tmp + h_len + 1, DEFAULT_KEY_NAME);
	sd.key_file = fopen(tmp, "rb");
	free(tmp);
	if(!sd.key_file ) {
		CRITICAL_ERROR(2,"Can't open default key file \"%s\"\n",
						STANDERT_KEY_FILE);
	}
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

void parse_init_rotor_str()
{
	int parsed = sscanf(sd.init_rotor_str,"%u;%u;%u",
			&sd.init_vals[0],&sd.init_vals[1],&sd.init_vals[2]);
	if(parsed != 3)
		CRITICAL_ERROR(3, "Initial values \"%s\" pars error",
						sd.init_rotor_str);
	if(sd.init_vals[0]>ROTOR_SZ || 
	  sd.init_vals[1]>ROTOR_SZ ||
	  sd.init_vals[2]>ROTOR_SZ)
		CRITICAL_ERROR(3, "One of initial values (\"%s\") is above %d",
						sd.init_rotor_str, ROTOR_SZ);
}

void crypt()
{
	if(enigma_fread(sd.key_file, &sd.eng)!=0)
		CRITICAL_ERROR(4,"Error while reading read key file. "
						"Maybe it's damagged.\n");
	if(enigma_verify(&sd.eng)!=0)
		CRITICAL_ERROR(4,"Error: bad key.\n");
	
	if(sd.init_rotor_str) {
		sd.eng.e_rotations[0]=sd.init_vals[0];
		sd.eng.e_rotations[1]=sd.init_vals[1];
		sd.eng.e_rotations[2]=sd.init_vals[2];
	}
	do_crypt();
}

void clean()
{
	fclose(sd.key_file);
	fclose(sd.inp_file);
	fclose(sd.outp_file);
	if(sd.init_rotor_str) free(sd.init_rotor_str);
	if(sd.key_file_name) free(sd.key_file_name);
	if(sd.inp_file_name) free(sd.inp_file_name);
	if(sd.outp_file_name) free(sd.outp_file_name);

}

void do_crypt()
{
	if(enigma_fcrypt(&sd.eng, sd.inp_file, sd.outp_file)!=0)
		CRITICAL_ERROR(5,"Error crypting file.");
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
