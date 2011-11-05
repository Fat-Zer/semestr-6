/**
 * AS-IS. 
 */

#ifndef FAT_ZER_AES_H
#define FAT_ZER_AES_H
#include <stdint.h>
// #define AES_FUNCTION_PREFIX my_
// #define AES_TYPE_PREFIX mu_
// #define AES_DECLARE_FUNCTION(funcname, ...) AES_FUNCTION_PREFIX##_##funcname(VA_ARGS)
// 
// AES_DECLARE_FUNCTION()
#define MYAES_MAXNR 14
#define MYAES_BLOCK_SIZE 16

typedef struct myaes_key_t {
    uint32_t rd[4 *(MYAES_MAXNR + 1)];
	uint8_t nr;
} MYAES_KEY;

void MYAES_set_encrypt_key(uint8_t *user_key, uint16_t sz, MYAES_KEY *key);
void MYAES_encrypt(const uint8_t* in, uint8_t* out, const MYAES_KEY* key);


#endif // FAT_ZER_AES_H
