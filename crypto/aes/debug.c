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

void print_block(char* title, const uint8_t* block)
{
	int i;
	puts(title);
	for(i=0;i<4;i++) {
		printf("%02X %02X %02X %02X\n",
				block[i*4+0], block[i*4+1],
				block[i*4+2], block[i*4+3]);
	}
}


void print_3block(char* title, const uint8_t* block0,
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
