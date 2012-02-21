PROJECT(descrypt)


ADD_EXECUTABLE(descrypt src/descrypt.c src/des.c src/cli_utils.c src/bit_reverse.c)
ADD_EXECUTABLE(destest src/test.c src/des.c src/des_work.c src/bit_reverse.c)

# ADD_EXECUTABLE(engcrypt src/genkey.c src/enigma.c)


