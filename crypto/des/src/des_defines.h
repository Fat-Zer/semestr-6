#ifndef DES_DEFINES_H
#define DES_DEFINES_H

#include "des.h"

#define RIGHT(block) (\
    ((block)[3] << 24) ^ ((block)[2] << 16) ^ \
    ((block)[1] <<  8) ^ ((block)[0]))

#define LEFT(block) (\
    ((block)[7] << 24) ^ ((block)[6] << 16) ^ \
    ((block)[5] <<  8) ^ ((block)[4]))

#endif // DES_DEFINES_H