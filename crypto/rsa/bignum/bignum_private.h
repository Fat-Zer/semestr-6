#ifndef BIGNUM_PRIVATE_H
#define BIGNUM_PRIVATE_H

#include <limits.h>
#include "bignum.h"

#define BN_INTERNAL

// TODO: make this makroses faster with shifts
/** bits in BN_word_t
 */
#define BN_WORD_BITS (CHAR_BIT * sizeof(BN_word_t))
/** The number of full words used by n-bit big number
 */
#define BN_FULL_WORDS(n) ((n) / BN_WORD_BITS)
/** how many bits is stored in the extra word
 */
#define BN_EXTRA_BITS(n) ((n) - BN_WORD_BITS * BN_FULL_WORDS(n))
/** how many words need to store the big number
 */
#define BN_NEED_WORDS(n) (((n+7) / CHAR_BIT + (sizeof(BN_word_t) -1)) / sizeof(BN_word_t))
/** Get bits from the high word
 */
#define BN_HIGH_INCOMPLITE_WORD(num) (BN_WORD_LOW_MASK(BN_EXTRA_BITS((num)->w)) ? (num)->d[BN_FULL_WORDS((num)->w)] & BN_WORD_LOW_MASK(BN_EXTRA_BITS((num)->w)) : 0 )

/** zero of type BN_word_t 
 */
#define BN_WORD_ZERO ((BN_word_t)0)
/** Maximum value of BN_word_t
 */
#define BN_WORD_MAX  (~BN_WORD_ZERO)
/** Mask where high n bits are ones
 */
#define BN_WORD_HIGH_MASK(n) (BN_WORD_MAX << (n))
/** Mask there low n bits are ones and other are zeros
 */
#define BN_WORD_LOW_MASK(n) (~BN_WORD_HIGH_MASK(n))

#define BN_WORD_LOW_BITS(w, n) (BN_WORD_LOW_MASK(n) & (w))

#define BN_WORD_IS_LOW_BITS_SET(w, n) (!(BN_WORD_LOW_MASK(n) == BN_WORD_LOW_BITS(w)))

#endif // BIGNUM_PRIVATE_H
