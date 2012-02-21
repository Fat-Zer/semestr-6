#!/bin/sh
CFLAGS="-g -O0 -Wall"
CC="gcc"


$CC $CFLAGS enigma.c genkey.c -o genkey
$CC $CFLAGS enigma.c engcrypt.c -o engcrypt


