#ifndef BIT_REVERSE_H
#define BIT_REVERSE_H
#include <stdint.h>
#include <stddef.h>

inline uint8_t reverse_bit_byte(uint8_t bite);
inline void reverse_bit_buff(uint8_t *dest, uint8_t *src, size_t sz);
inline uint64_t reverse_bits64(uint64_t val);
inline uint32_t reverse_bits32(uint32_t val);
inline uint8_t fast_byte_reverse(uint8_t byte);
inline uint8_t reverse_bits4(uint8_t val);


#endif // BIT_REVERSE_H
