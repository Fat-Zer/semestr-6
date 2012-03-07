#include <stdio.h>
#include <stdbool.h>
#include <openssl/aes.h>
#include "aes.h"
#include "debug.h"

#define test_block_print_error(i, ok, label, model, my) \
	do { \
		printf("Tesing %s: ", label); \
		for((i=0), (ok=1); i<0x100; i++) \
			if(model[i]!=my[i]) { \
				printf ("\n%02X should be %02X but it's %02X", i, model[i], my[i]); \
				ok=0; \
			} \
		if(!ok) { \
			puts(""); \
			print_block(label, 0x100 ,0x10, my, " ", "\n"); \
			return -1; \
		} else { \
			printf("%s ok\n", label); \
		} \
	} while(0)

void inv_mix_single_column(uint8_t *column);

int main(int argc, char **argv)
{
	uint8_t block[16] = {
		0x32, 0x43, 0xf6, 0xa8,
		0x88, 0x5a, 0x30, 0x8d,
		0x31, 0x31, 0x98, 0xa2,
		0xe0, 0x37, 0x07, 0x34};
 	
	uint8_t user_key[32] = {
		0x2b, 0x7e, 0x15, 0x16,
		0x28, 0xae, 0xd2, 0xa6,
		0xab, 0xf7, 0x15, 0x88,
		0x09, 0xcf, 0x4f, 0x3c};
	uint8_t target[16];
	uint8_t dtarget[16];
	MYAES_KEY my_key;

	uint8_t target2[16];
	uint8_t dtarget2[16];
	AES_KEY key;
	
	print_block_paralel("block", 1, 0x10 ,4, block);
	
	AES_set_encrypt_key(user_key, 128, &key);
	AES_encrypt(block, target2, &key);
	MYAES_set_encrypt_key(user_key, 4, &my_key);

	MYAES_encrypt(block, target, &my_key);
	print_block_paralel("encrypted", 2, 0x10 ,4, target, target2);
	
	AES_set_decrypt_key(user_key, 128, &key);
	AES_decrypt(target2, dtarget2, &key);
	MYAES_set_decrypt_key(user_key, 4, &my_key);
	MYAES_decrypt(target, dtarget, &my_key);
	
	print_block_paralel("decripted", 2, 0x10 ,4, dtarget, dtarget2);
	
	return 0;
}
