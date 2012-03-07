#include <stdlib.h>
#include <stdio.h>
#include "enigma.h"
#include "bit_array.h"

#define BLOCK_SZ (1024)

typedef unsigned char uchar;

unsigned my_rand() {
	FILE* f=fopen("/dev/random","r");
	unsigned rv;
	fread(&rv, sizeof(rv), 1,f);
	fclose(f);
	return rv;
}

int enigma_gen(struct enigma *eng)
{
	int i;
	int retval;
	for(i=0;i<ROTOR_NUM;i++)
	{
		if((retval=rotor_gen(&(eng->e_rotors[i])))!=0)
			return retval;
		eng->e_rotations[i]=0;
	}
	return reflector_gen(eng->e_refl);
}

int rotor_gen(struct rotor *rtr)
{
	int retval;
	if(!(retval = transpos_gen(rtr->r_fwd,ROTOR_SZ)))
		retval=transpos_reverse(rtr->r_back, rtr->r_fwd, ROTOR_SZ);
	return retval;
}

#define SWAP(TYPE,a,b) \
	{\
		TYPE t;\
		t=a;\
		a=b;\
		b=t;\
	}

void reflector_shuffle(uchar *rfl)
{
	size_t i;
	int cur=-1;
	for(i=0; i<ROTOR_SZ; i++)
		if(rfl[i]==i)
		{
			if(cur<0)
				cur=i;
			else
			{
				SWAP(uchar,rfl[i],rfl[cur]);
				cur=-1;
			}
		}
}

int reflector_gen(uchar *rfl)
{
//	unsigned uchar tmp[ROTOR_SZ];
	unsigned i,j,k;
	int retval;

	retval = transpos_gen(rfl,ROTOR_SZ);
	if(retval!=0)
		return retval;
	for(i=0; i<ROTOR_SZ; i++)
	{
		for(j=i+1;j<ROTOR_SZ; j++)
			if(rfl[j]==i)
				break;
		if(j!=ROTOR_SZ)
		{
			k = rfl[i];
			rfl[j] = rfl[k];
			rfl[k] = i;
		}
	}
	reflector_shuffle(rfl);
	return retval;
}

int transpos_gen(uchar *arr, int sz)
{
	int i,j;

	for(i=0; i<sz; i++)
	{
		j=my_rand()%(i+1);
		arr[i]=arr[j];
		arr[j]=i;
	}
	return 0;
}

int transpos_reverse(uchar *dst, uchar *src, size_t sz)
{
	unsigned i;
	for(i=0; i<sz; i++)
		dst[(int)src[i]] = i;
	return 0;
}


int enigma_fwrite(FILE* fs, struct enigma *eng)
{
	int i;
	int retval;

	for(i=0;i<ROTOR_NUM;i++)
	{
		retval = rotor_fwrite(fs, &eng->e_rotors[i]);
		if(retval)
			return retval;
	}
	return reflector_fwrite(fs, eng->e_refl);
}

int rotor_fwrite(FILE* fs, struct rotor *rtr)
{
	int retval;
	retval = transpos_fwrite(fs, rtr->r_fwd, ROTOR_SZ);
	return retval;
}

inline int reflector_fwrite(FILE* fs, uchar* refl)
{
	return transpos_fwrite(fs, refl, ROTOR_SZ);
}

inline int transpos_fwrite(FILE* fs, uchar *transp, size_t transp_sz)
{
	return fwrite(transp, sizeof(uchar), transp_sz, fs)==transp_sz*sizeof(uchar) ? 0 : 1;
}

int enigma_fread(FILE* fs, struct enigma *eng)
{
	int i;
	int retval;
	for(i=0;i<ROTOR_NUM;i++)
	{
		if((retval = rotor_fread(fs, &eng->e_rotors[i]))!=0)
			return retval;
		eng->e_rotations[i]=0;
	}
	
	return reflector_fread(fs, eng->e_refl);
}

int rotor_fread(FILE* fs, struct rotor *rtr)
{
	int retval;
	retval = transpos_fread(fs, rtr->r_fwd, ROTOR_SZ);
	if(retval==0)
		transpos_reverse(rtr->r_back,rtr->r_fwd,ROTOR_SZ);
	return retval;
}

inline int reflector_fread(FILE* fs, uchar* refl)
{
	return transpos_fread(fs, refl, ROTOR_SZ);
}

int transpos_fread(FILE* fs, uchar *transp, size_t transp_sz)
{
	return fread(transp, sizeof(uchar), transp_sz, fs)==transp_sz*sizeof(uchar) ? 0 : 1;
}

int enigma_verify(struct enigma *eng)
{
	int i;
	int retval;
	for(i=0;i<ROTOR_NUM;i++)
		if((retval = rotor_verify(&eng->e_rotors[i]))!=0)
			break;
	if(!retval)
		retval = reflector_verify(eng->e_refl);

	return retval;
}

int rotor_verify(struct rotor *rtr)
{
	int retval;
	retval = transpos_verify(rtr->r_fwd, ROTOR_SZ);
	if(retval==0)
		retval=transpos_reverse_verify(rtr->r_back,rtr->r_fwd,ROTOR_SZ);
	return retval;
}

int reflector_verify(uchar* refl)
{
	return transpos_reverse_verify(refl, refl, ROTOR_SZ);
}

int transpos_verify(uchar *transp, size_t transp_sz)
{
	struct bit_array *tr_map = alloc_bit_array(transp_sz);
	int rv=0;
	size_t i;
	
	for(i=0; i<transp_sz; i++)
	{
		if(transp[i]>=transp_sz)	{ rv = 1; break; }
		ba_set_bit(tr_map,transp[i]);
	}
	if(!rv)
		rv = ba_is_all_set(tr_map) ? 0 : 1;
	free(tr_map);
	return rv;
}

int transpos_reverse_verify(uchar *back, uchar *fwd, size_t sz)
{
	size_t i;
	for(i=0; i<sz; i++)
		if(back[(size_t)fwd[i]] != i)
			return i+1;
	return 0;
}


int enigma_fcrypt(struct enigma *eng, FILE *inf, FILE *outf)
{
	uchar buf[BLOCK_SZ];
	size_t cur_sz;
	int rv = 0;
	do {
		cur_sz = fread(buf, sizeof(uchar), BLOCK_SZ, inf);
		if(cur_sz != sizeof(uchar)*BLOCK_SZ && !feof(inf))
			{ rv = 1; break; }
		if(enigma_block_ncrypt(eng, buf, cur_sz)!=0)
			{ rv = 2; break; }
		if(fwrite(buf, sizeof(uchar), cur_sz, outf) != 
				sizeof(uchar) * cur_sz)
			{ rv = 3; break; }
	} while(cur_sz==BLOCK_SZ);
	return rv;
}

int enigma_block_ncrypt(struct enigma *eng, uchar buf[], size_t sz)
{
	size_t i;
	for(i=0; i<sz; i++)
		buf[i] = enigma_crypt(eng,buf[i]);
	return 0;
}

uchar enigma_crypt(struct enigma *eng, uchar ch)
{
	uchar retval = ch;
	size_t i;
	for(i=0; i<ROTOR_NUM; i++)
		retval = apply_rotor_fwd(&eng->e_rotors[i], retval);

	retval = apply_reflector(eng->e_refl, retval);
	for(i=ROTOR_NUM; i!=0; i--)	
		retval = apply_rotor_bck(&eng->e_rotors[i-1], retval);
		
	enigma_rotate_rotors(eng,1);
	
	return retval;
}

uchar apply_rotor_fwd(struct rotor* rtr, uchar ch)
{ return rtr->r_fwd[ch]; }

uchar apply_rotor_bck(struct rotor* rtr, uchar ch)
{ return rtr->r_back[ch]; }

uchar apply_reflector(uchar rfl[], uchar ch)
{ return rfl[ch]; }

void enigma_rotate_rotors(struct enigma *eng, size_t n)
{
	size_t i;
	for(i=0;i<ROTOR_NUM && n;i++) {
		eng->e_rotations[i] = (eng->e_rotations[i] + n) % ROTOR_SZ;
		n /= ROTOR_SZ;
	}
}
