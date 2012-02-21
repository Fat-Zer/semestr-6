#include <limits.h>
#include "des.h"
#include "bit_reverse.h"


#define ARR_SZ(arr) (sizeof(arr)/sizeof(arr[0]))
#define SHR64(x,n) ((uint64_t)(x) >> (n))
#define SHR32(x,n) ((uint32_t)(x) >> (n))


uint8_t des_s_reduce(uint8_t val, int i);
uint32_t des_s_transform(uint64_t val);
uint32_t des_f(uint32_t val, uint64_t k_i);
uint64_t des_crypt_round(uint64_t block, uint64_t k_i, enum crypt_action action);

/*** Some inline util functions
 */

inline uint64_t high_n_mask64(uint_fast8_t n)
{ return ~SHR64(~0LL, n); }

inline uint64_t low_n_mask64(uint_fast8_t n)
{ return ~((~0LL) << n); }

inline uint32_t high_n_mask32(uint_fast8_t n)
{ return ~SHR32(~0, n); }

inline uint32_t low_n_mask32(uint_fast8_t n)
{ return ~((~0) << n); }

inline uint64_t rol64(uint64_t var, unsigned n)
{
    return (var << n) | SHR64(high_n_mask64(n) & var, sizeof(var)*CHAR_BIT - n) ;
}

inline uint_fast32_t rol32_subn(uint_fast32_t var, uint_fast8_t n, uint_fast8_t sz)
{
    return  ((low_n_mask32(sz - n) & var) << n) |
            SHR32((low_n_mask32(sz) & ((~0) << (sz-n)) & var), (sz - n)) ;
}

inline uint32_t high64(uint64_t var)
{
    return (uint32_t) ((var & (~0LL << 32)) >> 32);
}

inline uint32_t low64(uint64_t var)
{
    return (uint32_t) var;
}

inline uint64_t bit_transform(uint64_t var, const uint8_t bit_trans[], size_t sz)
{
    int i;
    uint64_t rv=0;
    for(i=0; i<sz; i++)
        if(((uint64_t) 1LL <<bit_trans[i]) & var)
            rv |= 1LL<<i;
    return rv;
}

/// TODO: rewrite the function with bit arrayes...
inline bool check_even_sum8(uint8_t byte)
{
    static const uint8_t nbits7[] = {
        0, 1, 1, 2, 1, 2, 2, 3,
        1, 2, 2, 3, 2, 3, 3, 4,
        1, 2, 2, 3, 2, 3, 3, 4,
        2, 3, 3, 4, 3, 4, 4, 5,
        1, 2, 2, 3, 2, 3, 3, 4,
        2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5,
        3, 4, 4, 5, 4, 5, 5, 6,
        1, 2, 2, 3, 2, 3, 3, 4,
        2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5,
        3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5,
        3, 4, 4, 5, 4, 5, 5, 6,
        3, 4, 4, 5, 4, 5, 5, 6,
        4, 5, 5, 6, 5, 6, 6, 7};
    return (nbits7[byte & 0x7F] & 1) ^ (byte >> 7);
}

des_key_t des_byte2key(unsigned char *key)
{
	register des_key_t rv=0;
	register int i;
	for(i=0;i<sizeof(rv);i++)
		rv |= (des_key_t)reverse_bit_byte(*(key++)) << (i*8);
	return rv;
}

int des_fcrypt(FILE* outp_file, FILE *inp_file, des_key_t key, enum crypt_action action)
{
	size_t sz, wr_sz;
	/*des_block_t*/ uint64_t block;
	int rv=0;
	des_round_key_t k_arr[16];

    if(des_isbad_key(key))
        return DES_BADKEY_ERR;
    
	des_gen_ks(key, k_arr);
	
	do{
        block=0;
		sz = fread(&block, 1, sizeof(block), inp_file);
		if(sz==0) {
			if(!feof(inp_file))
				rv=DES_READ_ERR;
			break;
		}
		if((rv=des_block_crypt(&block, k_arr, action))!=0)
        {
            rv = DES_CRYPT_ERR;
            break;
        }
		wr_sz = fwrite(&block, 1, sizeof(block), outp_file);
		if(wr_sz<sz) {
			rv=DES_WRITE_ERR;
			break;
		}
	} while(sz==sizeof(block));
	return rv;
}

bool des_isbad_key(uint64_t key)
{
    union {
        uint8_t byte[8];
        uint64_t key;
    } *key_p = (void*) &key;
    int i;
    for(i=0; i<8; i++)
        if(!check_even_sum8(key_p->byte[i]))
            return true;
    return false;
}

void des_gen_ks(uint64_t key, uint64_t* k_arr)
{
	int i;
	uint32_t c=0;
	uint32_t d=0;

	static const uint8_t GC[] ={
		56,  48,  40,  32,  24,  16,   8,
		 0,  57,  49,  41,  33,  25,  17,
		 9,   1,  58,  50,  42,  34,  26,
		18,  10,   2,  59,  51,  43,  35};
	static const uint8_t GD[] ={
       	62,  54,  46,  38,  30,  22,  14,
		 6,  61,  53,  45,  37,  29,  21,
		13,   5,  60,  52,  44,  36,  28,
		20,  12,   4,  27,  19,  11,   3};

	static const uint8_t H[] ={
		13,  16,  10,  23,   0,   4,
		 2,  27,  14,   5,  20,   9,
		22,  18,  11,   3,  25,   7,
		15,   6,  26,  19,  12,   1,
		40,  51,  30,  36,  46,  54,
		29,  39,  50,  44,  32,  47,
		43,  48,  38,  55,  33,  52,
		45,  41,  49,  35,  28,  31 };

	static const uint8_t shifts2[]={
		0, 0, 1, 1,
		1, 1, 1, 1,
		0, 1, 1, 1,
		1, 1, 1, 0};
		
	c=bit_transform(key,GC,ARR_SZ(GC));
    d=bit_transform(key,GD,ARR_SZ(GD));

    for(i=0;i<16;i++)
	{
		if(shifts2[i])
			{ c = (c>>2) | (c<<26); d = (d>>2) | (d<<26);}
		else
			{ c = (c>>1) | (c<<27); d = (d>>1) | (d<<27);}
		c &= 0xFFFFFFF;
		d &= 0xFFFFFFF;
		rol32_subn(c, 1, 28);
		uint64_t t = ((uint64_t)d << 28) | c;
		k_arr[i] = bit_transform(t , H, ARR_SZ(H));
	}
}

int des_block_crypt(uint64_t *block, uint64_t* k_arr, enum crypt_action action)
{
	uint8_t ip[] = {
		57,  49,  41,  33,  25,  17,   9,   1,
		59,  51,  43,  35,  27,  19,  11,   3,
		61,  53,  45,  37,  29,  21,  13,   5,
		63,  55,  47,  39,  31,  23,  15,   7,
		56,  48,  40,  32,  24,  16,   8,   0,
		58,  50,  42,  34,  26,  18,  10,   2,
		60,  52,  44,  36,  28,  20,  12,   4,
		62,  54,  46,  38,  30,  22,  14,   6 };

	uint8_t rev_ip[] = {
		39,   7,  47,  15,  55,  23,  63,  31,
		38,   6,  46,  14,  54,  22,  62,  30,
		37,   5,  45,  13,  53,  21,  61,  29,
		36,   4,  44,  12,  52,  20,  60,  28,
		35,   3,  43,  11,  51,  19,  59,  27,
		34,   2,  42,  10,  50,  18,  58,  26,
		33,   1,  41,   9,  49,  17,  57,  25,
		32,   0,  40,   8,  48,  16,  56,  24 };
    int i;
	*block = bit_transform(*block, ip, ARR_SZ(ip));
    union {
        struct {
            uint32_t R;
            uint32_t L;
        } sep;
        uint64_t val;
    } *block_p = (void*) block;

	uint32_t L, R, temp;
	R = block_p->sep.L;
	L = block_p->sep.R;
	if(action == DES_ENCRYPT_ACT)
        for(i = 0; i < 16; i++)
        {
                temp = R ^ des_f(L, k_arr[i]);
                R = L;
                L = temp;
        }
    else // (action == DES_DECRYPT_ACT)
        for(i = 16 - 1; i >= 0; i--)
        {
                temp = L ^ des_f(R, k_arr[i]);
                L = R;
                R = temp;
        }
	block_p->sep.L = R;
	block_p->sep.R = L;
	
    *block = bit_transform(*block, rev_ip, ARR_SZ(rev_ip));
    return 0;
}

uint64_t des_crypt_round(uint64_t block, uint64_t k_i, enum crypt_action action)
{
    union {
        struct {
            uint32_t R;
            uint32_t L;
        } sep;
        uint64_t val;
    } *block_p = (void*)&block;
	if(action == DES_ENCRYPT_ACT) {
        uint32_t L_copy = block_p->sep.L;
        block_p->sep.L = block_p->sep.R;
        block_p->sep.R = L_copy ^ des_f(block_p->sep.R, k_i);
    } else { // (action == DECRYPT)
        uint32_t R_copy = block_p->sep.R;
        block_p->sep.R = block_p->sep.L;
        block_p->sep.L = R_copy ^ des_f(block_p->sep.L, k_i);
    }
    return block_p->val;
}

uint32_t des_f(uint32_t val, uint64_t k_i)
{
    uint8_t E[] = {
        31,   0,   1,   2,   3,   4,
        3,    4,   5,   6,   7,   8,
        7,    8,  9,  10,  11,  12,
        11,  12,  13,  14,  15,  16,
        15,  16,  17,  18,  19,  20,
        19,  20,  21,  22,  23,  24,
        23,  24,  25,  26,  27,  28,
        27,  28,  29,  30,  31,   0};

    uint8_t P[] = {
 /*00*/  15,   6,  19,  20,  28,  11,  27,  16,
 /*08*/   0,  14,  22,  25,   4,  17,  30,  9,
 /*16*/   1,   7,  23,  13,  31,  26,   2,   8,
 /*24*/  18,  12,  29,   5,  21,  10,   3,  24};
        
    uint64_t eval = k_i ^ bit_transform(val,E,ARR_SZ(E));

    eval = des_s_transform(eval);
    
    return reverse_bits32((uint32_t)bit_transform(eval, P, ARR_SZ(P)));
}

inline uint8_t get_sixtet(uint64_t val, int i)
{
    return  SHR64(((low_n_mask64((i+1)*6) ^ low_n_mask64(i*6)) & val), i*6);
}

uint32_t des_s_transform(uint64_t val)
{
    int i;
    uint32_t rv=0;
    for(i=0; i<8; i++)
	{
		uint8_t t=get_sixtet(val,i);
        rv |= (uint64_t)des_s_reduce(t, i) << (4*(7-i));
	}
	return rv;
}


uint8_t des_s_reduce(uint8_t val, int i)
{
    static uint8_t s[8][4][16] = {
        { { 14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7 },
          { 0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8 },
          { 4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0 },
          { 15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13 } },

        { { 15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10 },
          { 3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5 },
          { 0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15 },
          { 13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9 } },

        { { 10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8 },
          { 13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1 },
          { 13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7 },
          { 1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12 } },

        { { 7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15 },
          { 13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9 },
          { 10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4 },
          { 3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14 } },

        { { 2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9 },
          { 14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6 },
          { 4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14 },
          { 11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3 } },

        { { 12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11 },
          { 10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8 },
          { 9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6 },
          { 4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13 } },

        { { 4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1 },
          { 13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6 },
          { 1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2 },
          { 6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12 } },

        { { 13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7 },
          { 1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2 },
          { 7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8 },
          { 2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11 } } };

    register int x = (val >> 1) & 0x0F;
//	x = ((x&1)<<3) | ((x&2)<<1) | ((x&4)>>1) | ((x&8)>>3);
    register int y = ((val >> 5) & 0x01) | (val & 0x01);
//	y= ((y&1)<<1) | ((y&2)>>1);
    return s[i][y][x];
}


inline int des_fdecrypt(FILE* outp_file, FILE *inp_file, uint64_t key)
    { return des_fcrypt(outp_file, inp_file, key, DES_DECRYPT_ACT); }

inline int des_fencrypt(FILE* outp_file, FILE *inp_file, uint64_t key)
    { return des_fcrypt(outp_file, inp_file, key, DES_ENCRYPT_ACT); }

inline int des_block_encrypt(uint64_t *block, uint64_t* k_arr)
    { return des_block_crypt(block, k_arr, DES_ENCRYPT_ACT); }

inline int des_block_decrypt(uint64_t *block, uint64_t* k_arr)
    { return des_block_crypt(block, k_arr, DES_ENCRYPT_ACT); }

const char * des_err_string(enum des_error err)
{
    static const struct  {
        enum des_error err;
        const char * str;
    } err_map[] = {
        {DES_OK,        "Everething OK."},
        {DES_READ_ERR,  "File read error."},
        {DES_CRYPT_ERR, "Error while crypting block."},
        {DES_WRITE_ERR, "File write error."},
        {DES_BADKEY_ERR,"Bad key checksum."}};
    int i;
    for(i=0; i<ARR_SZ(err_map); i++)
        if(err_map[i].err==err)
            return err_map[i].str;
    return "Unknown error";
}


