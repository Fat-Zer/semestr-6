#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include "aes.h"

void inv_mix_single_column(uint8_t *column);

void usage(char * argv0) {
	fprintf(stderr,"Usage: %s -uhde --key_str <key_str> --128 --192 --256\n"
	"--ifile <input_file> --ofile <output_file>\n", argv0);
}

void help(char * argv0) {
	usage(argv0);
	fprintf(stderr,
		"-u\t--usage\tshow usage message\n"
		"-h\t--help\tshow this help message\n"
		"-e\t--encrypt\tperform encription rather than decription\n"
		"-d\t--decrypt\tperform decription rather than encription\n"
		"-k\t--ofile <key_str>\tset key_str>\n"
		"-i\t--ifile <input file>\tset input file>\n"
		"-o\t--ofile <output file>\tset output file>\n"
		"\t\t--128\tuse 128-bit key_str\n"
		"\t\t--192\tuse 192-bit key_str\n"
		"\t\t--256\tuse 256-bit key_str\n");
}

int main(int argc, char **argv)
{
	FILE *is;
	FILE *os;
	bool usage_flag=0, help_flag=0;
	bool decrypt=0;
	char* key_str=0;
	char* ifile=0;
	char* ofile=0;
	size_t key_lng=128;
	uint8_t user_key[32] = {};
	MYAES_KEY my_key;
	
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"key_str",		required_argument,	0,  'k' },
			{"usage",	no_argument,		0,  'u' },
			{"128",		no_argument,		0,  '4' },
			{"192",		no_argument,		0,  '6' },
			{"256",		no_argument,		0,  '8' },
			{"encrypt",	no_argument,		0,  'e' },
			{"decrypt",	no_argument,		0,  'd' },
			{"help",	no_argument,		0,  'h' },
			{"ifile",	required_argument,	0,  'i' },
			{"ofile",	required_argument,	0,  'o' },
			{0,			0,					0,  0 }};
		const char *opt_string="uhdek:i:o:";
		char c;
		c = getopt_long(argc, argv, opt_string,
						long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'u':
			usage_flag=1;
			break;
		case 'h':
			help_flag=1;
			break;
		case 'd':
			decrypt=1;
			break;
		case 'e':
			decrypt=0;
			break;
		case 'k':
			key_str=optarg;
			break;
		case 'i':
			ifile=optarg;
			break;
		case 'o':
			ofile=optarg;
			break;
		case '4':
			key_lng=128;
			break;
		case '6':
			key_lng=192;
			break;
		case '8':
			key_lng=256;
			ofile=optarg;
			break;
		default:
			usage(argv[0]);
			exit(1);
		}
	}

	if(help_flag) {
		help(argv[0]);
	} else if(usage_flag) {
		usage(argv[0]);
	} else if(!key_str) {
		fprintf(stderr,"No key_str set\n");
		usage(argv[0]);
		exit(2);
	} else {
		int scnd=0;
		switch (key_lng) {
		case 256:
			scnd += sscanf(key_str+9*7-1,"-%02x%02x%02x%02x", 
				user_key[4*7], &user_key[4*7+1], 
				user_key[4*7+2], &user_key[4*7+3]);

			scnd += sscanf(key_str+9*6-1,"-%02x%02x%02x%02x-", 
				&user_key[4*6], &user_key[4*6+1], 
				&user_key[4*6+2], &user_key[4*6+3]);
			scnd -=8;
		case 192:
			scnd += sscanf(key_str+9*5-1,"-%02x%02x%02x%02x-", 
				&user_key[4*5], &user_key[4*5+1], 
				&user_key[4*5+2], &user_key[4*5+3]);
			scnd += sscanf(key_str+9*4-1,"-%02x%02x%02x%02x-", 
				&user_key[4*4], &user_key[4*4+1], 
				&user_key[4*4+2], &user_key[4*4+3]);
			scnd -=8
		case 128:
			scnd += sscanf(key_str+9*3-1,"-%02x%02x%02x%02x-", 
				&user_key[4*3], &user_key[4*3+1], 
				&user_key[4*3+2], &user_key[4*3+3]);
			scnd += sscanf(key_str+9*2-1,"-%02x%02x%02x%02x-", 
				&user_key[4*2], &user_key[4*2+1], 
				&user_key[4*2+2], &user_key[4*2+3]);
			scnd += sscanf(key_str+9*1-1,"-%02x%02x%02x%02x-", 
				&user_key[4*1], &user_key[4*1+1], 
				&user_key[4*1+2], &user_key[4*1+3]);
			scnd += sscanf(key_str+9*0,"%02x%02x%02x%02x-", 
				&user_key[4*0], &user_key[4*0+1], 
				&user_key[4*0+2], &user_key[4*0+3]);
			break;
		default:
			fprintf(stderr,"No key_str set\n");
			usage();
			exit(3);
		} 
		if(scnd!=16) {
			fprintf(stderr,"Bad key_str.\n");
			usage();
			exit(4);
		}
	}

	if(ifile=0 || strcmp(ifile,"-")) {
		is = stdin;
	} else {
		is = fopen(ifile,"r");
		if(!is) {
			fprintf(stderr,"Failed to open input file:%s.\n"
			"Reason:%s\n", ifile strerr(errno));
			usage();
			exit(5);
		}
	}
	
	if(ofile=0 || strcmp(ofile,"-")) {
		os = stdout;
	} else {
		os = fopen(ofile,"w");
		if(!os) {
			fprintf(stderr,"Failed to open output file:%s.\n"
			"Reason:%s\n", ofile strerr(errno));
			usage();
			exit(6);
		}
	}
	

	MYAES_set_encrypt_key(user_key, 4, &my_key);
	uint8_t block[16] = {};
 	
	uint8_t target[16];


	MYAES_encrypt(block, target, &my_key);
	
	MYAES_set_decrypt_key(user_key, 4, &my_key);
	MYAES_decrypt(target, dtarget, &my_key);
	
	return 0;
}

