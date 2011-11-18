#include <stdint.h>
#include <assert.h>
#include <stdio.h>

#define WORD32(b1, b2, b3, b4) ( \
	((uint32_t)(b1) << 24) | ((uint32_t)(b2) << 16) | \
	((uint32_t)(b3) <<  8) | ((uint32_t)(b4) <<  0) )

// multiply with table
#define GF_MUL(a, b) ((a)&&(b) ? GF_ilog[ (GF_log[(a)] + GF_log[(b)]) % 0xff ] : 0)
// inverse according to table
// handle one and zero in  special way.
#define GF_INV(a)     ((a) ? GF_ilog[ (0xff^GF_log[(a)]) ? : 0xff] : 0)
#define AES_RPOL      (0x11b)	// reduction polynomial (x^8 + x^4 + x^3 + x + 1)
#define AES_GEN       (0x03)	// generator in the Rejindale's field
#define AES_SBOX_CC   (0x63)	// S-Box C constant (x^6 + x^5 + x + 1)
#define ROR(x, n, sz)	(((x)>>(n)) | ((x)<<((sz)-(n))))
#define ROL(x, n, sz)	(((x)<<(n)) | ((x)>>((sz)-(n))))
#define ROR32(x, n)	(ROR(x, n, 32))
#define ROL32(x, n)	(ROL(x, n, 32))
/** this function manualy multiplies numbers in GF(2^8) field.
 */
inline uint8_t GF_mul_manual(uint8_t a, uint8_t b)
{
	register uint8_t rv;

	rv = 0;
	while(b) {
		if(b & 0x01)
			rv ^= a;
		if(a & 0x80) { // if hight bit
			a <<= 1;
			a ^= AES_RPOL & 0xFF;
		} else {
			a <<= 1;
		}
		b >>= 1;
	}

	return rv;
}

/** This function generates log and inv_log(exponent) 
 * tables for Gf(2^8) field.
 *
 * This is done by multiplying field generator(0x03 here) 
 * for itselfe several times.
 * After this we can:
 *  - multiply numbers in this field like
 * this: a*b=invlog(log(a*b))=invlog(log(a)+log(b))
 *  - find reverse: a^(-1)=1/a=invlog(log(1/a))
 *  =invlog(log(1)-log(a))=invlog(log(1)+log(a))
 *
 * some notes:
 * 	- 0 have no logorithm, define it zero; also define inv_log(0) as 1
 * 	- 1 0x03^0xff==0x01==0x03^0; define it 0xff
 * 	- ff have no inv_log define it as 0x01 
 *		it's really a kind of a hack.
 */

void gen_GF_log_ilog(uint8_t* GF_log, uint8_t* GF_ilog)
{
	uint8_t gen = 1, i;
	assert(GF_log && GF_ilog);

	for(i = 0; i < 0xff; i++) {
		GF_log[gen] = i;
		GF_ilog[i] = gen;
		gen = GF_mul_manual(gen, AES_GEN);
	}
//	GF_log[gen] = 0xff;
	GF_ilog[0xff] = gen;

// special values 
	GF_log[0x00]  = 0x00;
//	GF_ilog[0x00] = 0x00;
	
}

/*** Return byte for b position in sbox table.
 */
uint8_t gen_AES_sbyte(const uint8_t *GF_log, const uint8_t *GF_ilog, uint8_t b)
{
	uint8_t rc=0;
	uint8_t i;
	unsigned char inv = GF_INV(b);

	assert(GF_log && GF_ilog);

	for(i = 0; i < 8; i++) {
		// based on transformation 5.1
		// could also be done with a loop based on the matrix
		rc |= ((inv & (1<<i)?1:0)
			 ^ (inv & (1 << ((i+4) & 7))?1:0)
		     ^ (inv & (1 << ((i+5) & 7))?1:0) 
		     ^ (inv & (1 << ((i+6) & 7))?1:0)
		     ^ (inv & (1 << ((i+7) & 7))?1:0) 
		     ^ (AES_SBOX_CC & (1 << i)?1:0)
			) << i;
		}
	return rc;	
}

/** Generates full sbox table and it's inverse.
 */
void gen_AES_sbox(const uint8_t *GF_log, const uint8_t *GF_ilog,
		uint8_t *sbox, uint8_t *isbox)
{
	int i;
	assert(sbox && isbox);

	for(i=0; i<0x100; i++)
	{
		sbox[i] = gen_AES_sbyte(GF_log,GF_ilog,i);
		isbox[sbox[i]] = i; 
	}
}


/** generates multiply table for value val
 */
void gen_GF_mul_table(const uint8_t *GF_log, const uint8_t *GF_ilog,
		uint8_t *tbl, uint8_t val)
{
	int i;
	for(i=0; i<0x100; i++)
		tbl[i]=GF_MUL(i,val);
}

/** generetes table for combine mix_columns and sbox steps this is for byte0
 * ETbl0 = S [x].[02, 01, 01, 03]; 
 * ETbl1 = S [x].[03, 02, 01, 01];
 * ETbl2 = S [x].[01, 03, 02, 01];
 * ETbl3 = S [x].[01, 01, 03, 02];
 */
void gen_AES_mixsbox_table(const uint8_t *GF_Mul2, const uint8_t *GF_Mul3, const uint8_t* sbox, 
		uint32_t *tbl, uint8_t tblnum)
{
	int i;
	for(i=0; i<0x100; i++) {
		uint8_t stmp=sbox[i];
		tbl[i] = ROL32(WORD32(GF_Mul3[stmp], stmp, stmp, GF_Mul2[stmp]), 8 * tblnum);

	}
}
/** generetes table for combine mix_columns and sbox steps this is for byte0
 * DTbl0[x] = Si[x].[0e, 09, 0d, 0b];
 * DTbl1[x] = Si[x].[0b, 0e, 09, 0d];
 * DTbl2[x] = Si[x].[0d, 0b, 0e, 09];
 * DTbl3[x] = Si[x].[09, 0d, 0b, 0e];
 */
void gen_AES_imixsbox_table(const uint8_t *GF_Mul9, const uint8_t *GF_MulB,
		const uint8_t *GF_MulD, const uint8_t *GF_MulE, 
		const uint8_t* isbox, uint32_t *tbl, uint8_t tblnum)
{
	int i;
	for(i=0; i<0x100; i++)
		tbl[i] = ROL32(WORD32(GF_MulB[isbox[i]], GF_MulD[isbox[i]], 
					GF_Mul9[isbox[i]], GF_MulE[isbox[i]]), 8 * tblnum);
}

void print_8bit_Ctbl(char* name, size_t sz, size_t by, const uint8_t* block)
{
	int i;

	printf("const uint8_t %s[] = {\n\t",name);
	for(i=0; i<sz-1; i++) {
			if((i+1)%by!=0)
				printf("0x%02X, ", block[i]);
			else
				printf("0x%02X,\n\t", block[i]);
		}
	printf("0x%02X};\n",block[sz-1]);
}

void print_32bit_Ctbl(char* name, size_t sz, size_t by, const uint32_t* block)
{
	int i;

	printf("const uint32_t %s[] = {\n\t",name);
	for(i=0; i<sz-1; i++) {
			if((i+1)%by!=0)
				printf("0x%08X, ", block[i]);
			else
				printf("0x%08X,\n\t", block[i]);
		}
	printf("0x%08X};\n",block[sz-1]);
}

#define PRINT_C_TABLE8(tbl, by) print_8bit_Ctbl(#tbl, sizeof(tbl), by, tbl);
#define PRINT_C_TABLE32(tbl, by) print_32bit_Ctbl(#tbl, sizeof(tbl) / 4, by, tbl);


int main(int argc, char **argv)
{
	uint8_t GF_log[256];
	uint8_t GF_ilog[256];
	uint8_t sbox[256];
	uint8_t isbox[256];
	uint8_t GF_Mul2[256];
	uint8_t GF_Mul3[256];
	uint8_t GF_Mul9[256];
	uint8_t GF_MulB[256];
	uint8_t GF_MulD[256];
	uint8_t GF_MulE[256];
	uint32_t sbox_mix0[256];
	uint32_t sbox_mix1[256];
	uint32_t sbox_mix2[256];
	uint32_t sbox_mix3[256];

	uint32_t isbox_mix0[256];
	uint32_t isbox_mix1[256];
	uint32_t isbox_mix2[256];
	uint32_t isbox_mix3[256];

	gen_GF_log_ilog(GF_log, GF_ilog);
	gen_AES_sbox(GF_log, GF_ilog, sbox, isbox);
	PRINT_C_TABLE8(sbox,16);
	PRINT_C_TABLE8(isbox,16);
	
	gen_GF_mul_table(GF_log, GF_ilog, GF_Mul2, 0x02);
	PRINT_C_TABLE8(GF_Mul2,16);
	gen_GF_mul_table(GF_log, GF_ilog, GF_Mul3, 0x03);
	PRINT_C_TABLE8(GF_Mul3,16);
	gen_GF_mul_table(GF_log, GF_ilog, GF_Mul9, 0x09);
	PRINT_C_TABLE8(GF_Mul9,16);
	gen_GF_mul_table(GF_log, GF_ilog, GF_MulB, 0x0b);
	PRINT_C_TABLE8(GF_MulB,16);
	gen_GF_mul_table(GF_log, GF_ilog, GF_MulD, 0x0d);
	PRINT_C_TABLE8(GF_MulD,16);
	gen_GF_mul_table(GF_log, GF_ilog, GF_MulE, 0x0e);
	PRINT_C_TABLE8(GF_MulE,16);

	gen_AES_mixsbox_table(GF_Mul2, GF_Mul3, sbox, sbox_mix0, 0);
	gen_AES_mixsbox_table(GF_Mul2, GF_Mul3, sbox, sbox_mix1, 1);
	gen_AES_mixsbox_table(GF_Mul2, GF_Mul3, sbox, sbox_mix2, 2);
	gen_AES_mixsbox_table(GF_Mul2, GF_Mul3, sbox, sbox_mix3, 3);

	PRINT_C_TABLE32(sbox_mix0, 8);
	PRINT_C_TABLE32(sbox_mix1, 8);
	PRINT_C_TABLE32(sbox_mix2, 8);
	PRINT_C_TABLE32(sbox_mix3, 8);

	gen_AES_imixsbox_table(GF_Mul9, GF_MulB, GF_MulD, GF_MulE, isbox, isbox_mix0, 0);
	gen_AES_imixsbox_table(GF_Mul9, GF_MulB, GF_MulD, GF_MulE, isbox, isbox_mix1, 1);
	gen_AES_imixsbox_table(GF_Mul9, GF_MulB, GF_MulD, GF_MulE, isbox, isbox_mix2, 2);
	gen_AES_imixsbox_table(GF_Mul9, GF_MulB, GF_MulD, GF_MulE, isbox, isbox_mix3, 3);

	PRINT_C_TABLE32(isbox_mix0, 8);
	PRINT_C_TABLE32(isbox_mix1, 8);
	PRINT_C_TABLE32(isbox_mix2, 8);
	PRINT_C_TABLE32(isbox_mix3, 8);
	
	return 0;
}

