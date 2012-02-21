#ifndef DES_WORK_H
#define DES_WORK_H

#include <stdint.h>

#define EN0 0	/* MODE == encrypt */
#define DE1 1	/* MODE == decrypt */
typedef struct
{
    uint32_t ek[32];
    uint32_t dk[32];
} des_ctx;

/* Encrypt several blocks in ECB mode. Caller is responsible for
short blocks. */
void des_enc ( des_ctx *dc, unsigned char *data, int blocks );
void des_dec ( des_ctx *dc, unsigned char *data, int blocks );
void des_key ( des_ctx *dc, unsigned char *key );
uint32_t des_work_f(uint32_t val, uint32_t* key);
uint32_t des_ssl_f(uint32_t val, uint32_t* key);

#endif // DES_WORK_H