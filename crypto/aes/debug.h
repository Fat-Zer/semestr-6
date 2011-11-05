#ifndef FAT_ZER_DEBUG_H
#define FAT_ZER_DEBUG_H
#include <stdint.h>
#include <stdio.h>


void print_block_paralel(char* title, int num, size_t sz, size_t by, ...);
void print_block(char* title, const uint8_t* block);
void print_3block(char* title, const uint8_t* block0,
		const uint8_t* block1, const uint8_t* block2);

#endif // FAT_ZER_DEBUG_H
