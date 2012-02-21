#include "des_work.h"

extern void deskey ( unsigned char *, int16_t );
/*	hexkey[8] MODE
* Sets the internal key register according to the hexadecimal
* key contained in the 8 bytes of hexkey, according to the DES,
* for encryption or decryption according to MODE.
*/
extern void usekey ( uint32_t * );
/* cookedkey[32]
* Loads the internal key register with the data in cookedkey.
*/
extern void cpkey ( uint32_t * );
/* cookedkey[32]
* Copies the contents of the internal key register into the storage
* located at &cookedkey[0].
*/
extern void des ( unsigned char *, unsigned char * );
/* from[8] to[8]
* Encrypts/Decrypts (according to the key currently loaded in the
* internal key register) one block of eight bytes at address 'from'
* into the block at address 'to'. They can be the same.
*/
static void scrunch ( unsigned char *, uint32_t * );
static void unscrun ( uint32_t *, unsigned char * );
static void desfunc ( uint32_t *, uint32_t * );
static void cookey ( uint32_t * );
uint32_t des_work_f(uint32_t val, uint32_t* key);

static uint32_t KnL[32] = { 0L };
static uint32_t KnR[32] = { 0L };
static uint32_t Kn3[32] = { 0L };
static unsigned char Df_Key[24] =
{
    0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,
    0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10,
    0x89,0xab,0xcd,0xef,0x01,0x23,0x45,0x67
};

static uint16_t bytebit[8] =
{
    0200, 0100, 040, 020, 010, 04, 02, 01
};

static uint32_t bigbyte[24] = {
	0x800000L, 0x400000L,  0x200000L,   0x100000L,
	0x80000L,  0x40000L,   0x20000L,    0x10000L,
	0x8000L,   0x4000L,    0x2000L,     0x1000L,
	0x800L,    0x400L,     0x200L,   	0x100L,
	0x80L,     0x40L,      0x20L,    	0x10L,
	0x8L,      0x4L,       0x2L,        0x1L};
/* Use the key schedule specified in the Standard (ANSI X3.92-1981). */
static unsigned char pc1[56] = {
    56, 48, 40, 32, 24, 16,  8,  0, 57, 49, 41, 33, 25, 17,
     9,  1, 58, 50, 42, 34, 26, 18, 10,  2, 59, 51, 43, 35,
    62, 54, 46, 38, 30, 22, 14,  6, 61, 53, 45, 37, 29, 21,
    13,  5, 60, 52, 44, 36, 28, 20, 12,  4, 27, 19, 11, 3};

static unsigned char totrot[16] =
{
    1,2,4,6,8,10,12,14,15,17,19,21,23,25,27,28
};

static unsigned char pc2[48] =
{
    13, 16, 10, 23, 0, 4,    2, 27, 14, 5, 20, 9,
    22, 18, 11, 3, 25, 7,    15, 6, 26, 19, 12, 1,
    40, 51, 30, 36, 46, 54,  29, 39, 50, 44, 32, 47,
    43, 48, 38, 55, 33, 52,  45, 41, 49, 35, 28, 31
};

void deskey ( key, edf ) /* Thanks to James Gillogly & Phil Karn! */
unsigned char *key;
int16_t edf;
{
    register int i, j, l, m, n;
    unsigned char pc1m[56], pcr[56];
    uint32_t kn[32];
    for ( j = 0; j < 56; j++ )
    {
        l = pc1[j];
        m = l & 07;
        pc1m[j] = ( key[l >> 3] & bytebit[m] ) ? 1 : 0;
    }
    for ( i = 0; i < 16; i++ )
    {
        if ( edf == DE1 ) m = ( 15 - i ) << 1;
        else m = i << 1;
        n = m + 1;
        kn[m] = kn[n] = 0L;
        for ( j = 0; j < 28; j++ )
        {
            l = j + totrot[i];
            if ( l < 28 ) pcr[j] = pc1m[l];
            else pcr[j] = pc1m[l - 28];
        }
        for ( j = 28; j < 56; j++ )
        {
            l = j + totrot[i];
			if ( l < 56 ) pcr[j] = pc1m[l];
			else pcr[j] = pc1m[l - 28];
		}
		for ( j = 0; j < 24; j++ )
		{
			if ( pcr[pc2[j]] ) kn[m] |= bigbyte[j];
			if ( pcr[pc2[j+24]] ) kn[n] |= bigbyte[j];
		}
	}
	cookey ( kn );
	return;
}

static void cookey ( raw1 )
register uint32_t *raw1;
{
    register uint32_t *cook, *raw0;
    uint32_t dough[32];
    register int i;

	cook = dough;
	for ( i = 0; i < 16; i++, raw1++ )
	{
		raw0 = raw1++;
		*cook    = ( *raw0 & 0x00fc0000L ) << 6;
		*cook   |= ( *raw0 & 0x00000fc0L ) << 10;
		*cook   |= ( *raw1 & 0x00fc0000L ) >> 10;
		*cook++ |= ( *raw1 & 0x00000fc0L ) >> 6;
		*cook    = ( *raw0 & 0x0003f000L ) << 12;
		*cook   |= ( *raw0 & 0x0000003fL ) << 16;
		*cook   |= ( *raw1 & 0x0003f000L ) >> 4;
		*cook++ |= ( *raw1 & 0x0000003fL );
	}
	usekey ( dough );
	return;
}

void cpkey ( into )
register uint32_t *into;
{
    register uint32_t *from, *endp;

	from = KnL, endp = &KnL[32];
	while ( from < endp ) *into++ = *from++;
	return;
}

void usekey ( from )
register uint32_t *from;
{
    register uint32_t *to, *endp;

	to = KnL, endp = &KnL[32];
	while ( to < endp ) *to++ = *from++;
	return;
}

void des ( inblock, outblock )
unsigned char *inblock, *outblock;
{
    uint32_t work[2];
    scrunch ( inblock, work );
    desfunc ( work, KnL );
	unscrun ( work, outblock );
	return;
}

static void scrunch ( outof, into )
register unsigned char *outof;
register uint32_t *into;
{
    *into    = ( *outof++ & 0xffL ) << 24;
	*into   |= ( *outof++ & 0xffL ) << 16;
    *into   |= ( *outof++ & 0xffL ) << 8;
    *into++ |= ( *outof++ & 0xffL );
    *into    = ( *outof++ & 0xffL ) << 24;
    *into   |= ( *outof++ & 0xffL ) << 16;
    *into   |= ( *outof++ & 0xffL ) << 8;
    *into 	|= ( *outof & 0xffL );
    return;
}


static void unscrun ( outof, into )
register uint32_t *outof;
register unsigned char *into;
{
    *into++ = ( *outof >> 24 ) & 0xffL;
    *into++ = ( *outof >> 16 ) & 0xffL;
    *into++ = ( *outof >> 8 )  & 0xffL;
    *into++ = *outof++         & 0xffL;
    *into++ = ( *outof >> 24 ) & 0xffL;
    *into++ = ( *outof >> 16 ) & 0xffL;
    *into++ = ( *outof >> 8 )  & 0xffL;
    *into   = *outof           & 0xffL;
    return;
}
static uint32_t SP1[64] =
{
    0x01010400L, 0x00000000L, 0x00010000L, 0x01010404L,
    0x01010004L, 0x00010404L, 0x00000004L, 0x00010000L,
    0x00000400L, 0x01010400L, 0x01010404L, 0x00000400L,
    0x01000404L, 0x01010004L, 0x01000000L, 0x00000004L,
    0x00000404L, 0x01000400L, 0x01000400L, 0x00010400L,
    0x00010400L, 0x01010000L, 0x01010000L, 0x01000404L,
    0x00010004L, 0x01000004L, 0x01000004L, 0x00010004L,
    0x00000000L, 0x00000404L, 0x00010404L, 0x01000000L,
    0x00010000L, 0x01010404L, 0x00000004L, 0x01010000L,
    0x01010400L, 0x01000000L, 0x01000000L, 0x00000400L,
    0x01010004L, 0x00010000L, 0x00010400L, 0x01000004L,
    0x00000400L, 0x00000004L, 0x01000404L, 0x00010404L,
    0x01010404L, 0x00010004L, 0x01010000L, 0x01000404L,
    0x01000004L, 0x00000404L, 0x00010404L, 0x01010400L,
    0x00000404L, 0x01000400L, 0x01000400L, 0x00000000L,
    0x00010004L, 0x00010400L, 0x00000000L, 0x01010004L
};

static uint32_t SP2[64] =
{
    0x80108020L, 0x80008000L, 0x00008000L, 0x00108020L,
    0x00100000L, 0x00000020L, 0x80100020L, 0x80008020L,
    0x80000020L, 0x80108020L, 0x80108000L, 0x80000000L,
    0x80008000L, 0x00100000L, 0x00000020L, 0x80100020L,
    0x00108000L, 0x00100020L, 0x80008020L, 0x00000000L,
    0x80000000L, 0x00008000L, 0x00108020L, 0x80100000L,
    0x00100020L, 0x80000020L, 0x00000000L, 0x00108000L,
    0x00008020L, 0x80108000L, 0x80100000L, 0x00008020L,
    0x00000000L, 0x00108020L, 0x80100020L, 0x00100000L,
    0x80008020L, 0x80100000L, 0x80108000L, 0x00008000L,
    0x80100000L, 0x80008000L, 0x00000020L, 0x80108020L,
    0x00108020L, 0x00000020L, 0x00008000L, 0x80000000L,
    0x00008020L, 0x80108000L, 0x00100000L, 0x80000020L,
    0x00100020L, 0x80008020L, 0x80000020L, 0x00100020L,
    0x00108000L, 0x00000000L, 0x80008000L, 0x00008020L,
    0x80000000L, 0x80100020L, 0x80108020L, 0x00108000L
};

static uint32_t SP3[64] =
{

	0x00000208L, 0x08020200L, 0x00000000L, 0x08020008L,
    0x08000200L, 0x00000000L, 0x00020208L, 0x08000200L,
    0x00020008L, 0x08000008L, 0x08000008L, 0x00020000L,
    0x08020208L, 0x00020008L, 0x08020000L, 0x00000208L,
    0x08000000L, 0x00000008L, 0x08020200L, 0x00000200L,
    0x00020200L, 0x08020000L, 0x08020008L, 0x00020208L,
    0x08000208L, 0x00020200L, 0x00020000L, 0x08000208L,
    0x00000008L, 0x08020208L, 0x00000200L, 0x08000000L,
    0x08020200L, 0x08000000L, 0x00020008L, 0x00000208L,
    0x00020000L, 0x08020200L, 0x08000200L, 0x00000000L,
    0x00000200L, 0x00020008L, 0x08020208L, 0x08000200L,
    0x08000008L, 0x00000200L, 0x00000000L, 0x08020008L,
    0x08000208L, 0x00020000L, 0x08000000L, 0x08020208L,
    0x00000008L, 0x00020208L, 0x00020200L, 0x08000008L,
    0x08020000L, 0x08000208L, 0x00000208L, 0x08020000L,
    0x00020208L, 0x00000008L, 0x08020008L, 0x00020200L
};

static uint32_t SP4[64] =
{
    0x00802001L, 0x00002081L, 0x00002081L, 0x00000080L,
    0x00802080L, 0x00800081L, 0x00800001L, 0x00002001L,
    0x00000000L, 0x00802000L, 0x00802000L, 0x00802081L,
    0x00000081L, 0x00000000L, 0x00800080L, 0x00800001L,
    0x00000001L, 0x00002000L, 0x00800000L, 0x00802001L,
    0x00000080L, 0x00800000L, 0x00002001L, 0x00002080L,
    0x00800081L, 0x00000001L, 0x00002080L, 0x00800080L,
    0x00002000L, 0x00802080L, 0x00802081L, 0x00000081L,
    0x00800080L, 0x00800001L, 0x00802000L, 0x00802081L,
    0x00000081L, 0x00000000L, 0x00000000L, 0x00802000L,
    0x00002080L, 0x00800080L, 0x00800081L, 0x00000001L,
    0x00802001L, 0x00002081L, 0x00002081L, 0x00000080L,
    0x00802081L, 0x00000081L, 0x00000001L, 0x00002000L,
    0x00800001L, 0x00002001L, 0x00802080L, 0x00800081L,
    0x00002001L, 0x00002080L, 0x00800000L, 0x00802001L,
    0x00000080L, 0x00800000L, 0x00002000L, 0x00802080L
};

static uint32_t SP5[64] =
{
    0x00000100L, 0x02080100L, 0x02080000L, 0x42000100L,
    0x00080000L, 0x00000100L, 0x40000000L, 0x02080000L,
    0x40080100L, 0x00080000L, 0x02000100L, 0x40080100L,
    0x42000100L, 0x42080000L, 0x00080100L, 0x40000000L,
    0x02000000L, 0x40080000L, 0x40080000L, 0x00000000L,
    0x40000100L, 0x42080100L, 0x42080100L, 0x02000100L,
    0x42080000L, 0x40000100L, 0x00000000L, 0x42000000L,
    0x02080100L, 0x02000000L, 0x42000000L, 0x00080100L,
    0x00080000L, 0x42000100L, 0x00000100L, 0x02000000L,
    0x40000000L, 0x02080000L, 0x42000100L, 0x40080100L,
    0x02000100L, 0x40000000L, 0x42080000L, 0x02080100L,
    0x40080100L, 0x00000100L, 0x02000000L, 0x42080000L,
    0x42080100L, 0x00080100L, 0x42000000L, 0x42080100L,
    0x02080000L, 0x00000000L, 0x40080000L, 0x42000000L,
    0x00080100L, 0x02000100L, 0x40000100L, 0x00080000L,
    0x00000000L, 0x40080000L, 0x02080100L, 0x40000100L
};

static uint32_t SP6[64] =
{
    0x20000010L, 0x20400000L, 0x00004000L, 0x20404010L,
    0x20400000L, 0x00000010L, 0x20404010L, 0x00400000L,
    0x20004000L, 0x00404010L, 0x00400000L, 0x20000010L,
    0x00400010L, 0x20004000L, 0x20000000L, 0x00004010L,
    0x00000000L, 0x00400010L, 0x20004010L, 0x00004000L,
    0x00404000L, 0x20004010L, 0x00000010L, 0x20400010L,
    0x20400010L, 0x00000000L, 0x00404010L, 0x20404000L,
    0x00004010L, 0x00404000L, 0x20404000L, 0x20000000L,
    0x20004000L, 0x00000010L, 0x20400010L, 0x00404000L,
    0x20404010L, 0x00400000L, 0x00004010L, 0x20000010L,
    0x00400000L, 0x20004000L, 0x20000000L, 0x00004010L,
    0x20000010L, 0x20404010L, 0x00404000L, 0x20400000L,
    0x00404010L, 0x20404000L, 0x00000000L, 0x20400010L,
    0x00000010L, 0x00004000L, 0x20400000L, 0x00404010L,
    0x00004000L, 0x00400010L, 0x20004010L, 0x00000000L,
    0x20404000L, 0x20000000L, 0x00400010L, 0x20004010L
};

static uint32_t SP7[64] =
{
    0x00200000L, 0x04200002L, 0x04000802L, 0x00000000L,
    0x00000800L, 0x04000802L, 0x00200802L, 0x04200800L,
    0x04200802L, 0x00200000L, 0x00000000L, 0x04000002L,
    0x00000002L, 0x04000000L, 0x04200002L, 0x00000802L,
    0x04000800L, 0x00200802L, 0x00200002L, 0x04000800L,
    0x04000002L, 0x04200000L, 0x04200800L, 0x00200002L,
    0x04200000L, 0x00000800L, 0x00000802L, 0x04200802L,
    0x00200800L, 0x00000002L, 0x04000000L, 0x00200800L,
    0x04000000L, 0x00200800L, 0x00200000L, 0x04000802L,
    0x04000802L, 0x04200002L, 0x04200002L, 0x00000002L,
    0x00200002L, 0x04000000L, 0x04000800L, 0x00200000L,
    0x04200800L, 0x00000802L, 0x00200802L, 0x04200800L,
    0x00000802L, 0x04000002L, 0x04200802L, 0x04200000L,
    0x00200800L, 0x00000000L, 0x00000002L, 0x04200802L,
    0x00000000L, 0x00200802L, 0x04200000L, 0x00000800L,
    0x04000002L, 0x04000800L, 0x00000800L, 0x00200002L
};

static uint32_t SP8[64] =
{
    0x10001040L, 0x00001000L, 0x00040000L, 0x10041040L,
    0x10000000L, 0x10001040L, 0x00000040L, 0x10000000L,
    0x00040040L, 0x10040000L, 0x10041040L, 0x00041000L,
    0x10041000L, 0x00041040L, 0x00001000L, 0x00000040L,
    0x10040000L, 0x10000040L, 0x10001000L, 0x00001040L,
    0x00041000L, 0x00040040L, 0x10040040L, 0x10041000L,
    0x00001040L, 0x00000000L, 0x00000000L, 0x10040040L,
    0x10000040L, 0x10001000L, 0x00041040L, 0x00040000L,
    0x00041040L, 0x00040000L, 0x10041000L, 0x00001000L,
    0x00000040L, 0x10040040L, 0x00001000L, 0x00041040L,
    0x10001000L, 0x00000040L, 0x10000040L, 0x10040000L,
    0x10040040L, 0x10000000L, 0x00040000L, 0x10001040L,
    0x00000000L, 0x10041040L, 0x00040040L, 0x10000040L,
    0x10040000L, 0x10001000L, 0x10001040L, 0x00000000L,
    0x10041040L, 0x00041000L, 0x00041000L, 0x00001040L,
    0x00001040L, 0x00040040L, 0x10000000L, 0x10041000L
};

static void desfunc ( block, keys )
register uint32_t *block, *keys;
{
    register uint32_t fval, work, right, leftt;
    register int round;

    leftt = block[0];
    right = block[1];
    work = ( ( leftt >> 4 ) ^ right ) & 0x0f0f0f0fL;
    right ^= work;
    leftt ^= ( work << 4 );
    work = ( ( leftt >> 16 ) ^ right ) & 0x0000ffffL;
    right ^= work;
    leftt ^= ( work << 16 );
    work = ( ( right >> 2 ) ^ leftt ) & 0x33333333L;
    leftt ^= work;
    right ^= ( work << 2 );
    work = ( ( right >> 8 ) ^ leftt ) & 0x00ff00ffL;
    leftt ^= work;
    right ^= ( work << 8 );
    right = ( ( right << 1 ) | ( ( right >> 31 ) & 1L ) ) & 0xffffffffL;
    work = ( leftt ^ right ) & 0xaaaaaaaaL;
    leftt ^= work;
    right ^= work;
    leftt = ( ( leftt << 1 ) | ( ( leftt >> 31 ) & 1L ) ) & 0xffffffffL;
    for ( round = 0; round < 8; round++ )
    {
		//leftt ^= des_ssl_f(right,keys);
		leftt ^= des_work_f(right,keys);
		keys+=2;
		//right ^= des_ssl_f(leftt,keys);
		right ^= des_work_f(leftt,keys);
		keys+=2;
/*        work  = ( right << 28 ) | ( right >> 4 );
        work ^= *keys++;
        fval  = SP7[   work         & 0x3fL];
        fval |= SP5[ ( work >>  8 ) & 0x3fL];
        fval |= SP3[ ( work >> 16 ) & 0x3fL];
        fval |= SP1[ ( work >> 24 ) & 0x3fL];
        work  = right ^ *keys++;
        fval |= SP8[ work           & 0x3fL];
		fval |= SP6[ ( work >>  8 ) & 0x3fL];
        fval |= SP4[ ( work >> 16 ) & 0x3fL];
        fval |= SP2[ ( work >> 24 ) & 0x3fL];
        leftt ^= fval;
        work  = ( leftt << 28 ) | ( leftt >> 4 );
        work ^= *keys++;
        fval  = SP7[   work         & 0x3fL];
        fval |= SP5[ ( work >>  8 ) & 0x3fL];
        fval |= SP3[ ( work >> 16 ) & 0x3fL];
        fval |= SP1[ ( work >> 24 ) & 0x3fL];
        work  = leftt ^ *keys++;
        fval |= SP8[   work         & 0x3fL];
        fval |= SP6[ ( work >>  8 ) & 0x3fL];
        fval |= SP4[ ( work >> 16 ) & 0x3fL];
        fval |= SP2[ ( work >> 24 ) & 0x3fL];
        right ^= fval;
 */   }
    right = ( right << 31 ) | ( right >> 1 );
    work = ( leftt ^ right ) & 0xaaaaaaaaL;
    leftt ^= work;
    right ^= work;
    leftt = ( leftt << 31 ) | ( leftt >> 1 );
    work = ( ( leftt >> 8 ) ^ right ) & 0x00ff00ffL;
    right ^= work;
    leftt ^= ( work << 8 );
    work = ( ( leftt >> 2 ) ^ right ) & 0x33333333L;
    right ^= work;
    leftt ^= ( work << 2 );
    work = ( ( right >> 16 ) ^ leftt ) & 0x0000ffffL;
    leftt ^= work;
    right ^= ( work << 16 );
    work = ( ( right >> 4 ) ^ leftt ) & 0x0f0f0f0fL;
    leftt ^= work;
    right ^= ( work << 4 );
	*block++ = right;
	*block = leftt;
	return;
}

/* Validation sets:
*
* Single-length key, single-length plaintext -
* Key
: 0123 4567 89ab cdef
* Plain : 0123 4567 89ab cde7
* Cipher : c957 4425 6a5e d31d
*
**********************************************************************/
void des_key ( des_ctx *dc, unsigned char *key )
{
    deskey ( key,EN0 );
    cpkey ( dc->ek );
    deskey ( key,DE1 );
    cpkey ( dc->dk );
}

/* Encrypt several blocks in ECB mode. Caller is responsible for
short blocks. */
void des_enc ( des_ctx *dc, unsigned char *data, int blocks )
{
    uint32_t work[2];
    int i;
    unsigned char *cp;

	cp = data;
	for ( i=0;i<blocks;i++ )
	{
		scrunch ( cp,work );
		desfunc ( work,dc->ek );
		unscrun ( work,cp );
		cp+=8;
	}
}

void des_dec ( des_ctx *dc, unsigned char *data, int blocks )
{
    uint32_t work[2];
    int i;
    unsigned char *cp;

	cp = data;
	for ( i=0;i<blocks;i++ )
	{
		scrunch ( cp,work );
		desfunc ( work,dc->dk );
		unscrun ( work,cp );
		cp+=8;
	}
}

uint32_t des_work_f(uint32_t val, uint32_t* key)
{
	uint32_t fval;
	uint32_t work;
	work  = ( val << 28 ) | ( val >> 4 );
	work ^= key[0];
	fval  = SP7[   work         & 0x3fL];
	fval |= SP5[ ( work >>  8 ) & 0x3fL];
	fval |= SP3[ ( work >> 16 ) & 0x3fL];
	fval |= SP1[ ( work >> 24 ) & 0x3fL];
	work  = val ^ key[1];
	fval |= SP8[   work         & 0x3fL];
	fval |= SP6[ ( work >>  8 ) & 0x3fL];
	fval |= SP4[ ( work >> 16 ) & 0x3fL];
	fval |= SP2[ ( work >> 24 ) & 0x3fL];
	return fval;
}


// ################################### from openssl ############################

#define LOAD_DATA_tmp(a,b,c,d,e,f) LOAD_DATA(a,b,c,d,e,f,g)
#define LOAD_DATA(R,S,u,t,E0,E1,tmp) \
	u=R^s[S  ]; \
	t=R^s[S+1]

#  define ROTATE(a,n)	({ register unsigned int ret;	\
				asm ("rorl %1,%0"	\
					: "=r"(ret)	\
					: "I"(n),"0"(a)	\
					: "cc");	\
			   ret;				\
			})
			
static const uint32_t DES_SPtrans[8][64]={
{
/* nibble 0 */
0x02080800L, 0x00080000L, 0x02000002L, 0x02080802L,
0x02000000L, 0x00080802L, 0x00080002L, 0x02000002L,
0x00080802L, 0x02080800L, 0x02080000L, 0x00000802L,
0x02000802L, 0x02000000L, 0x00000000L, 0x00080002L,
0x00080000L, 0x00000002L, 0x02000800L, 0x00080800L,
0x02080802L, 0x02080000L, 0x00000802L, 0x02000800L,
0x00000002L, 0x00000800L, 0x00080800L, 0x02080002L,
0x00000800L, 0x02000802L, 0x02080002L, 0x00000000L,
0x00000000L, 0x02080802L, 0x02000800L, 0x00080002L,
0x02080800L, 0x00080000L, 0x00000802L, 0x02000800L,
0x02080002L, 0x00000800L, 0x00080800L, 0x02000002L,
0x00080802L, 0x00000002L, 0x02000002L, 0x02080000L,
0x02080802L, 0x00080800L, 0x02080000L, 0x02000802L,
0x02000000L, 0x00000802L, 0x00080002L, 0x00000000L,
0x00080000L, 0x02000000L, 0x02000802L, 0x02080800L,
0x00000002L, 0x02080002L, 0x00000800L, 0x00080802L,
},{
/* nibble 1 */
0x40108010L, 0x00000000L, 0x00108000L, 0x40100000L,
0x40000010L, 0x00008010L, 0x40008000L, 0x00108000L,
0x00008000L, 0x40100010L, 0x00000010L, 0x40008000L,
0x00100010L, 0x40108000L, 0x40100000L, 0x00000010L,
0x00100000L, 0x40008010L, 0x40100010L, 0x00008000L,
0x00108010L, 0x40000000L, 0x00000000L, 0x00100010L,
0x40008010L, 0x00108010L, 0x40108000L, 0x40000010L,
0x40000000L, 0x00100000L, 0x00008010L, 0x40108010L,
0x00100010L, 0x40108000L, 0x40008000L, 0x00108010L,
0x40108010L, 0x00100010L, 0x40000010L, 0x00000000L,
0x40000000L, 0x00008010L, 0x00100000L, 0x40100010L,
0x00008000L, 0x40000000L, 0x00108010L, 0x40008010L,
0x40108000L, 0x00008000L, 0x00000000L, 0x40000010L,
0x00000010L, 0x40108010L, 0x00108000L, 0x40100000L,
0x40100010L, 0x00100000L, 0x00008010L, 0x40008000L,
0x40008010L, 0x00000010L, 0x40100000L, 0x00108000L,
},{
/* nibble 2 */
0x04000001L, 0x04040100L, 0x00000100L, 0x04000101L,
0x00040001L, 0x04000000L, 0x04000101L, 0x00040100L,
0x04000100L, 0x00040000L, 0x04040000L, 0x00000001L,
0x04040101L, 0x00000101L, 0x00000001L, 0x04040001L,
0x00000000L, 0x00040001L, 0x04040100L, 0x00000100L,
0x00000101L, 0x04040101L, 0x00040000L, 0x04000001L,
0x04040001L, 0x04000100L, 0x00040101L, 0x04040000L,
0x00040100L, 0x00000000L, 0x04000000L, 0x00040101L,
0x04040100L, 0x00000100L, 0x00000001L, 0x00040000L,
0x00000101L, 0x00040001L, 0x04040000L, 0x04000101L,
0x00000000L, 0x04040100L, 0x00040100L, 0x04040001L,
0x00040001L, 0x04000000L, 0x04040101L, 0x00000001L,
0x00040101L, 0x04000001L, 0x04000000L, 0x04040101L,
0x00040000L, 0x04000100L, 0x04000101L, 0x00040100L,
0x04000100L, 0x00000000L, 0x04040001L, 0x00000101L,
0x04000001L, 0x00040101L, 0x00000100L, 0x04040000L,
},{
/* nibble 3 */
0x00401008L, 0x10001000L, 0x00000008L, 0x10401008L,
0x00000000L, 0x10400000L, 0x10001008L, 0x00400008L,
0x10401000L, 0x10000008L, 0x10000000L, 0x00001008L,
0x10000008L, 0x00401008L, 0x00400000L, 0x10000000L,
0x10400008L, 0x00401000L, 0x00001000L, 0x00000008L,
0x00401000L, 0x10001008L, 0x10400000L, 0x00001000L,
0x00001008L, 0x00000000L, 0x00400008L, 0x10401000L,
0x10001000L, 0x10400008L, 0x10401008L, 0x00400000L,
0x10400008L, 0x00001008L, 0x00400000L, 0x10000008L,
0x00401000L, 0x10001000L, 0x00000008L, 0x10400000L,
0x10001008L, 0x00000000L, 0x00001000L, 0x00400008L,
0x00000000L, 0x10400008L, 0x10401000L, 0x00001000L,
0x10000000L, 0x10401008L, 0x00401008L, 0x00400000L,
0x10401008L, 0x00000008L, 0x10001000L, 0x00401008L,
0x00400008L, 0x00401000L, 0x10400000L, 0x10001008L,
0x00001008L, 0x10000000L, 0x10000008L, 0x10401000L,
},{
/* nibble 4 */
0x08000000L, 0x00010000L, 0x00000400L, 0x08010420L,
0x08010020L, 0x08000400L, 0x00010420L, 0x08010000L,
0x00010000L, 0x00000020L, 0x08000020L, 0x00010400L,
0x08000420L, 0x08010020L, 0x08010400L, 0x00000000L,
0x00010400L, 0x08000000L, 0x00010020L, 0x00000420L,
0x08000400L, 0x00010420L, 0x00000000L, 0x08000020L,
0x00000020L, 0x08000420L, 0x08010420L, 0x00010020L,
0x08010000L, 0x00000400L, 0x00000420L, 0x08010400L,
0x08010400L, 0x08000420L, 0x00010020L, 0x08010000L,
0x00010000L, 0x00000020L, 0x08000020L, 0x08000400L,
0x08000000L, 0x00010400L, 0x08010420L, 0x00000000L,
0x00010420L, 0x08000000L, 0x00000400L, 0x00010020L,
0x08000420L, 0x00000400L, 0x00000000L, 0x08010420L,
0x08010020L, 0x08010400L, 0x00000420L, 0x00010000L,
0x00010400L, 0x08010020L, 0x08000400L, 0x00000420L,
0x00000020L, 0x00010420L, 0x08010000L, 0x08000020L,
},{
/* nibble 5 */
0x80000040L, 0x00200040L, 0x00000000L, 0x80202000L,
0x00200040L, 0x00002000L, 0x80002040L, 0x00200000L,
0x00002040L, 0x80202040L, 0x00202000L, 0x80000000L,
0x80002000L, 0x80000040L, 0x80200000L, 0x00202040L,
0x00200000L, 0x80002040L, 0x80200040L, 0x00000000L,
0x00002000L, 0x00000040L, 0x80202000L, 0x80200040L,
0x80202040L, 0x80200000L, 0x80000000L, 0x00002040L,
0x00000040L, 0x00202000L, 0x00202040L, 0x80002000L,
0x00002040L, 0x80000000L, 0x80002000L, 0x00202040L,
0x80202000L, 0x00200040L, 0x00000000L, 0x80002000L,
0x80000000L, 0x00002000L, 0x80200040L, 0x00200000L,
0x00200040L, 0x80202040L, 0x00202000L, 0x00000040L,
0x80202040L, 0x00202000L, 0x00200000L, 0x80002040L,
0x80000040L, 0x80200000L, 0x00202040L, 0x00000000L,
0x00002000L, 0x80000040L, 0x80002040L, 0x80202000L,
0x80200000L, 0x00002040L, 0x00000040L, 0x80200040L,
},{
/* nibble 6 */
0x00004000L, 0x00000200L, 0x01000200L, 0x01000004L,
0x01004204L, 0x00004004L, 0x00004200L, 0x00000000L,
0x01000000L, 0x01000204L, 0x00000204L, 0x01004000L,
0x00000004L, 0x01004200L, 0x01004000L, 0x00000204L,
0x01000204L, 0x00004000L, 0x00004004L, 0x01004204L,
0x00000000L, 0x01000200L, 0x01000004L, 0x00004200L,
0x01004004L, 0x00004204L, 0x01004200L, 0x00000004L,
0x00004204L, 0x01004004L, 0x00000200L, 0x01000000L,
0x00004204L, 0x01004000L, 0x01004004L, 0x00000204L,
0x00004000L, 0x00000200L, 0x01000000L, 0x01004004L,
0x01000204L, 0x00004204L, 0x00004200L, 0x00000000L,
0x00000200L, 0x01000004L, 0x00000004L, 0x01000200L,
0x00000000L, 0x01000204L, 0x01000200L, 0x00004200L,
0x00000204L, 0x00004000L, 0x01004204L, 0x01000000L,
0x01004200L, 0x00000004L, 0x00004004L, 0x01004204L,
0x01000004L, 0x01004200L, 0x01004000L, 0x00004004L,
},{
/* nibble 7 */
0x20800080L, 0x20820000L, 0x00020080L, 0x00000000L,
0x20020000L, 0x00800080L, 0x20800000L, 0x20820080L,
0x00000080L, 0x20000000L, 0x00820000L, 0x00020080L,
0x00820080L, 0x20020080L, 0x20000080L, 0x20800000L,
0x00020000L, 0x00820080L, 0x00800080L, 0x20020000L,
0x20820080L, 0x20000080L, 0x00000000L, 0x00820000L,
0x20000000L, 0x00800000L, 0x20020080L, 0x20800080L,
0x00800000L, 0x00020000L, 0x20820000L, 0x00000080L,
0x00800000L, 0x00020000L, 0x20000080L, 0x20820080L,
0x00020080L, 0x20000000L, 0x00000000L, 0x00820000L,
0x20800080L, 0x20020080L, 0x20020000L, 0x00800080L,
0x20820000L, 0x00000080L, 0x00800080L, 0x20020000L,
0x20820080L, 0x00800000L, 0x20800000L, 0x20000080L,
0x00820000L, 0x00020080L, 0x20020080L, 0x20800000L,
0x00000080L, 0x20820000L, 0x00820080L, 0x00000000L,
0x20000000L, 0x20800080L, 0x00020000L, 0x00820080L,
}};

// D_ENCRYPT(LL,R,S)
uint32_t des_ssl_f(uint32_t val, uint32_t* key)
{
	uint32_t rv=0;
	uint32_t u, t;
	
	u=val^key[0];
	t=val^key[1];
	
	t=ROTATE(t,4);
	rv ^= DES_SPtrans[0][(u>> 2L)&0x3f];
	rv ^= DES_SPtrans[2][(u>>10L)&0x3f];
	rv ^= DES_SPtrans[4][(u>>18L)&0x3f];
	rv ^= DES_SPtrans[6][(u>>26L)&0x3f];
	rv ^= DES_SPtrans[1][(t>> 2L)&0x3f];
	rv ^= DES_SPtrans[3][(t>>10L)&0x3f];
	rv ^= DES_SPtrans[5][(t>>18L)&0x3f];
	rv ^= DES_SPtrans[7][(t>>26L)&0x3f];

	return rv;
	
}
