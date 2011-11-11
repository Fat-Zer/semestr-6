#ifndef FAT_ZER_DEBUG_H
#define FAT_ZER_DEBUG_H
#include <stdint.h>
#include <stdio.h>


void print_block_paralel(char* title, int num, size_t sz, size_t by, ...);
void print_3aes_block(char* title, const uint8_t* block0,
		const uint8_t* block1, const uint8_t* block2);
void print_aes_block(char* title, const uint8_t* block);
void print_block(char* title, size_t size, uint32_t by, const uint8_t* block,
		const char* col_sep, const char* raw_sep);
void print_block32(char* title, size_t sz, uint32_t by, const uint32_t* block,
		const char* col_sep, const char* raw_sep);

#endif // FAT_ZER_DEBUG_H
