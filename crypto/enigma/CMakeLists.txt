PROJECT(enigma)

ADD_EXECUTABLE(genkey src/genkey.c src/enigma.c src/bit_array.c)
ADD_EXECUTABLE(engcrypt src/engcrypt.c src/enigma.c src/bit_array.c
		src/cli_utils.c)

# ADD_EXECUTABLE(engcrypt src/genkey.c src/enigma.c)
