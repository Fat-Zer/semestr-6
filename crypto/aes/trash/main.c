// AES usage example
// compile as: gcc main.c aes.h aes.c

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "aes.h"
#include <stdarg.h>

void print_block_paralel(char* title, int num, size_t sz, size_t by, ...)
{
	int i, j, k;
	int linenum = (sz + by - 1) / by;
	va_list vl;

	puts(title);
	for(i=0; i<linenum; i++) {
		va_start(vl,by);
		for(j=0; j<num; j++) {
			uint8_t *buf = va_arg(vl,uint8_t*);
			for(k=0; k<by; k++)
				printf("%02X ", buf[i*by+k]);
			printf("     ");
		}
		va_end(vl);
		putchar('\n');
	}
}


int main(int argc, char *argv[])
{
//	unsigned char key[KEY_128] = {
//		0xff, 0xfe, 0xfd, 0xfc,
//		0xfb, 0xfa, 0xf9, 0xf8,
//		0xf7, 0xf6, 0xf5, 0xf4,
//		0xf3, 0xf2, 0xf1, 0xf0
//	};
	unsigned char key[16] = {
		0x00, 0x01, 0x02, 0x03,
		0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b,
		0x0c, 0x0d, 0x0e, 0x0f
	};
	
//	unsigned char ptext[16] = {
//		0x00, 0x01, 0x02, 0x03,
//		0x04, 0x05, 0x06, 0x07,
//		0x08, 0x09, 0x0a, 0x0b,
//		0x0c, 0x0d, 0x0e, 0x0f
//	};
	unsigned char ptext[16] = {
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};
	unsigned char ctext[16];
	unsigned char decptext[16];
	aes_ctx_t *ctx;
	int i;

	init_aes();
	ctx = aes_alloc_ctx(key, sizeof(key));
	if(!ctx) {
		perror("aes_alloc_ctx");
		return EXIT_FAILURE;
	}
	print_block_paralel("keys ", 1, 0x40, 4,
		ctx->keysched);
	print_block_paralel("keys ", 3, 16, 4,
		ptext, ctext, decptext);
	aes_encrypt(ctx, ptext, ctext);
	aes_decrypt(ctx, ctext, decptext);
	i = 16;
	print_block_paralel("plaintext ciphertext dectext ", 3, 16, 4,
		ptext, ctext, decptext);
	
	aes_free_ctx(ctx);
	return EXIT_SUCCESS;
}
