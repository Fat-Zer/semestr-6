#include <stdarg.h>
#include "debug.h"

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

void print_block(char* title, size_t sz, uint32_t by, const uint8_t* block,
		const char* col_sep, const char* raw_sep)
{
	int i;

	puts(title);
	for(i=0; i<sz-1; i++) {
			if((i+1)%by!=0)
				printf("%02X%s", block[i], col_sep);
			else
				printf("%02X%s", block[i], raw_sep);
		}
	printf("%02X\n",block[sz-1]);
}

void print_block32(char* title, size_t sz, uint32_t by, const uint32_t* block,
		const char* col_sep, const char* raw_sep)
{
	int i;

	puts(title);
	for(i=0; i<sz-1; i++) {
			if((i+1)%by!=0)
				printf("%08X%s", block[i], col_sep);
			else
				printf("%08X%s", block[i], raw_sep);
		}
	printf("%08X\n",block[sz-1]);
}

void print_aes_block(char* title, const uint8_t* block)
{
	int i;
	puts(title);
	for(i=0;i<4;i++) {
		printf("%02X %02X %02X %02X\n",
				block[i*4+0], block[i*4+1],
				block[i*4+2], block[i*4+3]);
	}
}

void print_aes_words(char* title, 
		const uint32_t w1, const uint32_t w2,
		const uint32_t w3, const uint32_t w4)
{
	puts(title);
	printf("%02X %02X %02X %02X\n",
			(w1      ) & 0xff, (w1 >>  8) & 0xff,
			(w1 >> 16) & 0xff, (w1 >> 24) & 0xff);
	printf("%02X %02X %02X %02X\n",
			(w2      ) & 0xff, (w2 >>  8) & 0xff,
			(w2 >> 16) & 0xff, (w2 >> 24) & 0xff);
	printf("%02X %02X %02X %02X\n",
			(w3      ) & 0xff, (w3 >>  8) & 0xff,
			(w3 >> 16) & 0xff, (w3 >> 24) & 0xff);
	printf("%02X %02X %02X %02X\n",
			(w4      ) & 0xff, (w4 >>  8) & 0xff,
			(w4 >> 16) & 0xff, (w4 >> 24) & 0xff);
}


void print_3aes_block(char* title, const uint8_t* block0,
		const uint8_t* block1, const uint8_t* block2)
{
	int i;
	puts(title);
	for(i=0;i<4;i++) {
		printf("%02X %02X %02X %02x      ",
				block0[i*4+0], block0[i*4+1],
				block0[i*4+2], block0[i*4+3]);
		printf("%02X %02X %02X %02X      ",
				block1[i*4+0], block1[i*4+1],
				block1[i*4+2], block1[i*4+3]);
		printf("%02x %02X %02X %02X",
				block2[i*4+0], block2[i*4+1],
				block2[i*4+2], block2[i*4+3]);
		putchar('\n');
	}
}
