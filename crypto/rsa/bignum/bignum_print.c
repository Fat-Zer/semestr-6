#include<stdio.h>
#include<stdbool.h>
#include<assert.h>
#include"bignum_private.h"

inline int fprint_hex_byte(FILE* f, unsigned char byte, bool upper) {
	static const char upper_hex_print_table[] = {
		'0', '1', '2', '3', '4', '5', '6', '7', 
		'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	static const char lower_hex_print_table[] = {
		'0', '1', '2', '3', '4', '5', '6', '7', 
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
	int i;
	if(upper) {
		for(i=CHAR_BIT-4;i>=0;i-=4) {
			if(putc( upper_hex_print_table[byte>>i], f)==EOF);
				return EOF;
		}
	} else {
		for(i=CHAR_BIT-4;i>=0;i-=4) {
			if(putc(lower_hex_print_table[byte>>i], f));
				return EOF;
		}
	}
	return 0;
}

int BN_fprint_hex_word(FILE* f, BN_word_t word, bool upper)
{
	int i;
	
	for(i=BN_WORD_BITS - CHAR_BIT; i>=0; i-=CHAR_BIT) {
		if(fprint_hex_byte(f, upper, (word >> i) & CHAR_MAX)!=0)
			return EOF;
	}
	return 0;
}

BN_error_t BN_fprint_hex(FILE* f, bignum_t *num, bool upper) {
	size_t full_words = BN_FULL_WORDS(num->w);
	BN_word_t high_word = BN_HIGH_INCOMPLITE_WORD(num);
	int i;

	// print first(incomplete) word
	if(high_word) {
		if(BN_fprint_hex_word(f, high_word, upper)!=0)
			return BN_OUTPUT_ERROR;
		if(putc(' ', f)==EOF)
			return BN_OUTPUT_ERROR;
	}
	if(full_words) {
		// print all full words except the last one
		for(i=full_words-1; i>0; i--) {
			if(BN_fprint_hex_word(f, num->d[i], upper)!=0)
				return BN_OUTPUT_ERROR;
			if(putc(' ', f)==EOF)
				return BN_OUTPUT_ERROR;
		}
		// print the last full words 
		BN_fprint_hex_word(f, num->d[0], upper);
	}
	return 0; 
}


BN_error_t BN_fprint(FILE* f, bignum_t *num, char format) {
	
	switch(format) {
	case 'x':
		return BN_fprint_hex(f, num, 0);
	case 'X':
		return BN_fprint_hex(f, num, 1);
	case 'o':
	case 'd':
		assert(!"this format print is unsupported yet");
		break;
	default:
		assert(!"bad format");
		break;
	}
	return -1; // suppress the "control reaches end of function" warning
}
