#include <stddef.h>
#include <stdbool.h>

typedef unsigned char ba_alloc_t;

/** A struct to represent bit array abstraction
 */
struct bit_array
{
	size_t nbits;
	ba_alloc_t arr[];
};

/** Allocates a bit_array struct with malloc()
 */
struct bit_array* alloc_bit_array(size_t nbits);

void ba_set_bit(struct bit_array* ba, size_t bitn);
void ba_reset_bit(struct bit_array* ba, size_t bitn);
bool ba_isset(struct bit_array* ba, size_t bitn);

size_t bytes_for_store_bits(size_t nbits);

bool ba_is_all_set(struct bit_array* ba);

