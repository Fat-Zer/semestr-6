#include <string.h>
#include "bignum.h"
#include "bignum_private.h"

#define MY_MAX(a,b)  ((a)>(b) ? (a) : (b))
//================= init/deinit functions =====================================

void BN_init(bignum_t *num) {
	num->d=0;
	num->w=0;
	num->sz=0;
}

bignum_t* BN_new() {
	bignum_t *rv;
	rv = malloc(sizeof(bignum_t));
	if(rv)
		BN_init(rv);

	return rv;
}

void BN_init(bignum_t *num) {
	num->d=0;
	num->w=0;
	num->sz=0;
}

void BN_deinit(bignum_t *num) {
	if(num->d) {
		free(num->d);
		num->d=0;
	}
	num->w=0;
	num->sz=0;
}

void BN_free(bignum_t* num) {
	if(num) {
		BN_deinit(num);
		free(num);
	}
}

BN_error_t BN_set_width(bignum_t* num, size_t new_w) {
	BN_error_t rc=0;

	if(new_w < 1) { // forbit negative and zero width
		rc = BN_WIDTH_CANT_BE_ZERO;
	} else if(new_w > num->w) {
		BN_word_t *new_d;
		size_t new_sz;
	
		new_sz = BN_NEED_WORDS(new_w);
		new_d = realloc(num, new_sz * sizeof(BN_word_t));
	
		if(new_d) {
			size_t old_w = num->w;
			num->d = new_d;
			num->sz = new_sz;
			num->w = new_w;
			BN_zero_head(num, old_w);
		} else {
			rc=BN_MALLOC_FAILED;
		}
	} else {
		num->w = new_w;
	}

	return rc;
}

BN_INTERNAL void BN_zero_head(bignum_t *num, size_t from) {
	if(from < num->w) {
		size_t from_word = BN_FULL_WORDS(from);
		size_t from_bits_first = from - from_word*BN_WORD_BITS;
		size_t i;

		if(from_bits_first) { // if "from" not on the border of word
			num->d[from_word] &= BN_WORD_LOW_MASK(from_bits_first);
			from_word++;
		}

		for(i=from_word; i<BN_NEED_WORDS(num->w); i++)
			num->d[i] = 0;
	}
}

// void BN_zero_headword_extrabits(bignum_t *num) {
// 	
// }
//================= copy/set functions ========================================

bignum_t* BN_zero(bignum_t* num) {
	memset(num->d, 0, BN_NEED_WORDS(num->w)*sizeof(BN_word_t));
	
	return num;
}

bignum_t* BN_cpy(bignum_t* num, bignum_t* from) {
	size_t cpy_cnt=BN_NEED_WORDS(MY_MAX(num->w,from->w))*sizeof(BN_word_t);
	memcpy(num->d, from->d, cpy_cnt*sizeof(BN_word_t));

	return num;
}

bignum_t* BN_cpyw(bignum_t* num, BN_word_t from) {
	*(num->d) = from;
	BN_zero_head(num, sizeof(BN_word_t));

	return num;
}
/*
bignum_t* BN_cpysw(bignum_t* num, BN_signed_word_t from) {
	if(num->w!=0) {
		if(from >= 0) {
			*(num->d) = from;
			BN_zero_head(num, sizeof(BN_word_t));
		} else {
			*(num->d) = -from;
			BN_FF_head()
		}
	}
	return num;
}
*/
//================= inc/dev functions =========================================

bignum_t* BN_inc(bignum_t* num) {
	size_t i;
	
	(*num->d)++;
	for(i=1; (i < BN_NEED_WORDS(num->w) - 1) && (num->d[i-1]==0); i++)
		num->d[i]++;
	
	return num;
}

bignum_t* BN_dec(bignum_t* num) {
	size_t i;
	
	(*num->d)--;
	for(i=1; (i < BN_NEED_WORDS(num->w) - 1) && (num->d[i-1]==BN_WORD_MAX); i++)
		num->d[i]--;

	return num;
}

// 
// bignum_t* BN_dec(bignum_t* num) {
// 	if(num->w!=0) {
// 		size_t i;
// 		
// 		num->sign ? (*num->d)++ : (*num->d)--;
// 		for(i=0; i<BN_NEED_WORDS(num->w) - 1; i++)
// 			if(num->d[i]==0)
// 				num->sign ? (*num->d[i+1])++ : (*num->d[i+1])--;
// 			else
// 				break;
// 	}
// 
// 	return num;
// }
// 
// BN_arr_inc(bignum_word_t)
