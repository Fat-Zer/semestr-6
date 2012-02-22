#include <stdio.h>
#include "bignum.h"

int main()
{
	int i;
	bignum_t* bna;
	bignum_t* bnb;
	bna = BN_new();
	bnb = BN_new();
	BN_set_width(bna,128);
	BN_set_width(bnb,128);
	
	BN_cpyw(bna,~0L);
	BN_fprint(stdout, bna, 'x');
	BN_fprint(stdout, bnb, 'X');
	BN_dec(bna);
	BN_fprint(stdout, bna, 'x');

	BN_inc(BN_inc(bna));
	BN_fprint(stdout, bna, 'X');

	BN_free(bna);
	BN_free(bnb);
	return 0;
}
