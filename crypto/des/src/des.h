#ifndef DES_H
#define DES_H
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


typedef enum des_error{
	DES_OK=0,
	DES_READ_ERR,
	DES_CRYPT_ERR,
	DES_WRITE_ERR,
	DES_BADKEY_ERR
} des_err_t;

typedef uint8_t des_block_t[8];

typedef uint64_t des_key_t;

typedef uint64_t des_round_key_t;

typedef  des_key_arr_t[8];

des_key_t des_byte2key(unsigned char *key);
void des_gen_ks(des_key_t key, uint64_t* k_arr);


/*
#define C2LL(ll,c) ll = \
        (uint64_t) c[0] << 56 | (uint64_t) c[1] << 48 \
        (uint64_t) c[2] << 40 | (uint64_t) c[3] << 32 \
        (uint64_t) c[4] << 24 | (uint64_t) c[5] << 16 \
        (uint64_t) c[6] <<  8 | (uint64_t) c[7]
*/

enum crypt_action { DES_ENCRYPT_ACT=1, DES_DECRYPT_ACT=0};

int des_fcrypt(FILE* outp_file, FILE *inp_file, des_key_t key, enum crypt_action action);
int des_block_crypt(uint64_t *block, uint64_t* k_arr, enum crypt_action action);
bool des_isbad_key(uint64_t key);

inline int des_fdecrypt(FILE* outp_file, FILE *inp_file, uint64_t key);
inline int des_fencrypt(FILE* outp_file, FILE *inp_file, uint64_t key);
inline int des_block_encrypt(uint64_t *block, uint64_t* k_arr);
inline int des_block_decrypt(uint64_t *block, uint64_t* k_arr);

const char * des_err_string(enum des_error err);


#endif // DES_H
