#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include "bit_array.h"

#define BA_ALLOC_T_BIT (CHAR_BIT*sizeof(ba_alloc_t))
#define BA_ALLOC_T_MASK ((ba_alloc_t)~0)
#define BA_NUMBER_MASK (BA_ALLOC_T_BIT - 1)
#define BA_NUMBER_SHIFT (3) // log2(BA_ALLOC_T_BIT)

struct bit_array* alloc_bit_array(size_t nbits)
{
	size_t arr_sz = bytes_for_store_bits(nbits);
	struct bit_array *rv = 
		(struct bit_array*) malloc(arr_sz + sizeof(size_t));
	rv->nbits = nbits;
	memset(rv->arr, 0, arr_sz);
	
	return rv;
}

void ba_set_bit(struct bit_array* ba, size_t bitn)
{
	if(bitn < ba->nbits)
	{
		size_t pos = bitn >> BA_NUMBER_SHIFT;
		ba_alloc_t bit = 1<<(bitn & BA_NUMBER_MASK);
		ba->arr[pos]|=bit;
	}
}

void ba_reset_bit(struct bit_array* ba, size_t bitn)
{
	if(bitn < ba->nbits)
	{
		size_t pos = bitn >> BA_NUMBER_SHIFT;
		ba_alloc_t bit = 1<<(bitn & BA_NUMBER_MASK);
		ba->arr[pos]&=~bit;
	}
}

bool ba_isset(struct bit_array* ba, size_t bitn)
{
	bool rv = 0;
	if(bitn < ba->nbits)
	{
		size_t pos = bitn >> BA_NUMBER_SHIFT;
		ba_alloc_t bit = 1<<(bitn & BA_NUMBER_MASK);
		rv=ba->arr[pos] & bit;
	}
	return rv;
}

size_t bytes_for_store_bits(size_t nbits)
{
	return ((nbits >> BA_NUMBER_SHIFT) +
			(BA_NUMBER_MASK & nbits?1:0));
}

ba_alloc_t last_byte_mask(size_t nbits)
{
	return (nbits & (BA_NUMBER_MASK)) ? 
				~0 : ~0 >> (BA_NUMBER_MASK - (nbits & BA_NUMBER_MASK));
}

bool ba_is_all_set(struct bit_array* ba)
{
	size_t i;
	size_t arr_sz = bytes_for_store_bits(ba->nbits);
	bool rv = 1;

	for(i=0; i<arr_sz-1; i++)
		if(BA_ALLOC_T_MASK & ~ba->arr[i])
		{
			rv = 0;
			break;
		}
	if(rv)
	{
		ba_alloc_t lbm = last_byte_mask(ba->nbits);
		rv = ( (ba->arr[arr_sz-1] & lbm) == lbm);
	}
	return rv;
}






