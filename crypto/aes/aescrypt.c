#include <stdio.h>
#include "aes.h"
#include "debug.h"
#include <openssl/aes.h>

int main(int argc, char **argv)
{
	uint8_t block[16] = {
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};
	
//	uint8_t block[16] = {
//		0x00, 0x01, 0x02, 0x03,
//		0x04, 0x05, 0x06, 0x07,
//		0x08, 0x09, 0x0a, 0x0b,
//		0x0c, 0x0d, 0x0e, 0x0f
//	};

	uint8_t user_key[16] = {
		0x00, 0x01, 0x02, 0x03,
		0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b,
		0x0c, 0x0d, 0x0e, 0x0f
	};
// 	uint8_t user_key[16] = {
// 		0xff, 0xfe, 0xfd, 0xfc,
// 		0xfb, 0xfa, 0xf9, 0xf8,
// 		0xf7, 0xf6, 0xf5, 0xf4,
// 		0xf3, 0xf2, 0xf1, 0xf0
// 	};
	
	uint8_t target1[16];
	uint8_t target2[16];
	AES_KEY key;
	MYAES_KEY my_key;
	
	AES_set_encrypt_key(user_key, 128, &key);
	MYAES_set_encrypt_key(user_key, 4, &my_key);
	print_block_paralel("generated keys", 2, 0x50 ,4, key.rd_key, my_key.rd);
	// print_block_paralel("openssl/AES encription",3, 16 ,4, block, target1, user_key);
	// AES_set_decrypt_key(user_key, 128, &key);
	// AES_decrypt(target1, target2 , &key);
	// print_3block("openssl/AES decrypted",target1, target2, user_key);

	AES_encrypt(block, target1, &key);
	MYAES_encrypt(block, target2, &my_key);
	print_block_paralel("encription", 2, 0x10 ,4, target1, target2);
	// print_3block("openssl/AES encription",block, target1, user_key);
	// AES_set_decrypt_key(user_key, 128, &key);
	// AES_decrypt(target1, target2 , &key);
	// print_3block("openssl/AES decrypted",target1, target2, user_key);
	
	return 0;
}
