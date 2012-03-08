#include "bignum_private.h"

#define MY_MAX(a,b) ((a)>(b)?(a):(b));
#define MY_MIN(a,b) ((a)<(b)?(a):(b));

int BN_add(bignum_t* a. const bignum_t* b) {
	size_t i, min;
	BN_word_t par=0;
	for(i=1, min = MY_Min (BN_NEED_WORDS(a->w)-1, BN_NEED_WORDS(b->w)-1); 
			i < max ; i++) {
		a->d[i]+=b->d[i] + par;
		if(a->d[i] > b->d[i]) {
			par=0;
		} else {
			par=1;
		}
	}
	return BN_OK;
}
