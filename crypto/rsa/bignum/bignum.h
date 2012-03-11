#ifndef BIGNUM_H_FAT_ZER
#define BIGNUM_H_FAT_ZER
/* This module implements big number arithmetics
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint64_t BN_word_t;
typedef int64_t BN_signed_word_t;

typedef enum BN_error_t {
	BN_OK = 0,
	BN_MALLOC_FAILED,
	BN_OUTPUT_ERROR,
	BN_BAD_VALUE,
	BN_WIDTH_CANT_BE_ZERO
} BN_error_t;

/** Structure that represents big number
 * @d - pointer to actual data, array of bignum_word_t
 * @w - actual size of useful data in the array (in bits)
// * @h - head bit
 * @sz - size of memory that was allocated for the number
 * @sign - if zero the number is positive. if not - it's negative
 */

typedef struct bignum_t {
	BN_word_t *d;
//	BN_word_t h;
	size_t w;
	size_t sz;
} bignum_t;

/** Initialize the big number
 * @n - pointer to number
 */
void BN_init(bignum_t *num);

/** Allocate new big number
 * @return pointer to allocated big number
 */
bignum_t * BN_new();

/** deinitialize and free the bignum pointed by num
 *
 */
void BN_free(bignum_t* num);

/** Free the data and set all pointers to zero
 * @num - the pointer to big number;
 */
void BN_deinit(bignum_t *num);

/** Zero all bits from @from to the end of the number
 * @num - the pointer to big number;
 * @from - the bit number starting from what we zero all other
 */
void BN_zero_head(bignum_t *num, size_t from);

/** Set the width of the number in bits
 * @num - the pointer to big number;
 * @new_w - new width of the number
 */
BN_error_t BN_set_width(bignum_t* num, size_t new_w);

/** Zero the number
 * @num -the pointer to big number;
 * @return *num;
 */
bignum_t* BN_zero(bignum_t* num);

/** Increment the given big number
 * @num - the pointer to big number
 * @return - pointer to incremented number
 */
bignum_t* BN_inc(bignum_t* num);

/** Decrement the given big number
 * @num - the pointer to big number
 * @return - pointer to decremented number
 */
bignum_t* BN_dec(bignum_t* num);

/** Print the big number to the specified stream
 * @f - file stream
 * @num - the big number
 * @format is a character, one of:
 *  - 'x' - lower hexadecimal
 *  - 'X' - upper hexadecimal
 *  @return returns BN_OUTPUT_ERROR if output fails otherwise returns 0
 */
BN_error_t BN_fprint(FILE* f, bignum_t *num, char format);

/** Print the big number to the stream specified in hex
 * @f - file stream
 * @num - the big number
 * @upper - 0 for lower-case letters and 1 for upper
 *  @return returns BN_OUTPUT_ERROR if output fails otherwise returns 0
 */
BN_error_t BN_fprint_hex(FILE* f, bignum_t *num, bool upper);

#endif // ! BIGNUM_H_FAT_ZER
