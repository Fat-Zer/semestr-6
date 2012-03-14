#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include "aes.h"

#ifdef DDEBUG
#	define DUBUG_PRINT(str, ...) fprintf(stderr, str, __VA_ARGS__)
#else
#	define DUBUG_PRINT(str, ...) 
#endif

void inv_mix_single_column(uint8_t *column);
void do_crypt_block(const uint8_t* blk, uint8_t* trg, MYAES_KEY* key, bool decrypt);
void do_encrypt_incomplete_block(uint8_t* blk, uint8_t* trg, MYAES_KEY* key, size_t incompl_n);
void do_fencrypt(FILE* ifs, FILE* ofs, MYAES_KEY* key);
void do_fdecrypt(FILE* ifs, FILE* ofs, MYAES_KEY* key);

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
			scnd += sscanf(key_str+9*7-1,"-%02hhx%02hhx%02hhx%02hhx", 
				&user_key[4*7], &user_key[4*7+1], 
				&user_key[4*7+2], &user_key[4*7+3]);
			scnd += sscanf(key_str+9*6-1,"-%02hhx%02hhx%02hhx%02hhx-", 
				&user_key[4*6], &user_key[4*6+1], 
				&user_key[4*6+2], &user_key[4*6+3]);
			scnd -=8;
		case 192:
			scnd += sscanf(key_str+9*5-1,"-%02hhx%02hhx%02hhx%02hhx-", 
				&user_key[4*5], &user_key[4*5+1], 
				&user_key[4*5+2], &user_key[4*5+3]);
			scnd += sscanf(key_str+9*4-1,"-%02hhx%02hhx%02hhx%02hhx-", 
				&user_key[4*4], &user_key[4*4+1], 
				&user_key[4*4+2], &user_key[4*4+3]);
			scnd -=8;
		case 128:
			scnd += sscanf(key_str+9*3-1,"-%02hhx%02hhx%02hhx%02hhx-", 
				&user_key[4*3], &user_key[4*3+1], 
				&user_key[4*3+2], &user_key[4*3+3]);
			scnd += sscanf(key_str+9*2-1,"-%02hhx%02hhx%02hhx%02hhx-", 
				&user_key[4*2], &user_key[4*2+1], 
				&user_key[4*2+2], &user_key[4*2+3]);
			scnd += sscanf(key_str+9*1-1,"-%02hhx%02hhx%02hhx%02hhx-", 
				&user_key[4*1], &user_key[4*1+1], 
				&user_key[4*1+2], &user_key[4*1+3]);
			scnd += sscanf(key_str+9*0,"%02hhx%02hhx%02hhx%02hhx-", 
				&user_key[4*0], &user_key[4*0+1], 
				&user_key[4*0+2], &user_key[4*0+3]);
			break;
		default:
			fprintf(stderr,"No key_str set\n");
			usage(argv[0]);
			exit(3);
		} 
		if(scnd!=16) {
			fprintf(stderr,"Bad key_str.\n");
			DUBUG_PRINT("scnd=%ld\n", scnd);
			usage(argv[0]);
			exit(4);
		}
	}

	if(ifile==0 || strcmp(ifile,"-")==0) {
		is = stdin;
	} else {
		is = fopen(ifile,"rb");
		if(!is) {
			fprintf(stderr,"Failed to open input file:%s.\n"
			"Reason:%s\n", ifile, strerror(errno));
			usage(argv[0]);
			exit(5);
		}
	}
	
	if(ofile==0 || strcmp(ofile,"-")==0) {
		os = stdout;
	} else {
		os = fopen(ofile,"wb");
		if(!os) {
			fprintf(stderr,"Failed to open output file:%s.\n"
			"Reason:%s\n", ofile, strerror(errno));
			usage(argv[0]);
			exit(6);
		}
	}
	if(decrypt==0) {
		MYAES_set_encrypt_key(user_key, key_lng/32, &my_key);
		do_fencrypt(is, os, &my_key);
	} else {
		MYAES_set_decrypt_key(user_key, key_lng/32, &my_key);
		do_fdecrypt(is, os, &my_key);
	}
	
 	
	fclose(is);
	fclose(os);
	
	return 0;
}

void do_fencrypt(FILE* ifs, FILE* ofs, MYAES_KEY* key) {
	uint8_t block[16];
	uint8_t target[16];
	size_t rdd;
	int rv;
		
	while (1) {
		rdd = fread(block, 1, sizeof(block), ifs);
		if(rdd!=sizeof(block)) {
			if(!feof(ifs)) {
				fprintf(stderr,"Failed to read file. Reason: %s\n", strerror(errno));
				exit(7);
			} else {
				break;
			}
		}
		do_crypt_block(block, target, key, 0);
		rv = fwrite(target, 1, sizeof(target), ofs);
		if(rv!=sizeof(block)) {
			fprintf(stderr,"Failed to write file. Reason: %s\n", strerror(errno));
			exit(9);
		}
	} 

	do_encrypt_incomplete_block(block, target, key, rdd);
	rv = fwrite(target, 1, sizeof(target), ofs);
	if(rv!=sizeof(target)) {
		fprintf(stderr,"Failed to write file. Reason: %s\n", strerror(errno));
		exit(9);
	}
}


void do_fdecrypt(FILE* ifs, FILE* ofs, MYAES_KEY* key) {
	uint8_t *block=malloc(16) , *last=malloc(16), *tmp;
	uint8_t target[16];
	size_t rdd;
	int rv;
	
	rdd = fread(last, 1, 16, ifs);
	if(rdd!=16 && !feof(ifs)) {
		fprintf(stderr,"Failed to read file. Reason: %s\n", strerror(errno));
		exit(7);
	}
		
	while (rdd==16) {
		do_crypt_block(last, target, key, 1);
		rv = fwrite(target, 1, 16, ofs);
		if(rv!=16) {
			fprintf(stderr,"Failed to write file. Reason: %s\n", strerror(errno));
			exit(9);
		}
		tmp=block; block=last; last=tmp;
		rdd = fread(block, 1, 16, ifs);
		if(rdd!=16 && !feof(ifs)) {
			fprintf(stderr,"Failed to read file. Reason: %s\n", strerror(errno));
			exit(7);
		}
	} 

	do_crypt_block(block, target, key, 1);
	if (target[16-1]>15) {
		fprintf(stderr,"Bad format\n");
		exit(10);
	}
	if(target[16-1]!=0) {
		rv = fwrite(target, 1, target[16-1], ofs);
		if(rv!=target[16-1]) {
			fprintf(stderr,"Failed to write file. Reason: %s\n", strerror(errno));
			exit(9);
		}
	}
}

void do_encrypt_incomplete_block(uint8_t* blk, uint8_t* trg, MYAES_KEY* key, size_t incompl_n) { 
	size_t i;
	srand(time(0));
	for(i=incompl_n; i<16-2; i++) {
		blk[i] = rand();
	}
	blk[15] = incompl_n;
	do_crypt_block(blk, trg, key, 0);
}

void do_crypt_block(const uint8_t* blk, uint8_t* trg, MYAES_KEY* key, bool decrypt) { 
	
	if(!decrypt) {
		MYAES_encrypt(blk, trg, key);
	} else {
		MYAES_decrypt(blk, trg, key);
	}
}
