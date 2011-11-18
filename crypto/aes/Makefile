CC = gcc
CFLAGS = -Wall -g -DAES_TABLE_ENCRIPTION
# CFLAGS = -Wall -O2

# aescrypt_LDFLAGS = -lcrypto
aescrypt_OBJECTS = aescrypt.o aes.o debug.o

aesdatagen_OBJECTS = aes_data_gen.o

TARGETS = aescrypt aesdatagen

RM = rm -f
INSTALL = /usr/bin/install -c

BINDIR = /usr/local/bin

all: $(TARGETS)

aescrypt: $(aescrypt_OBJECTS)
	$(CC) $(LDFLAGS) $(aescrypt_LDFLAGS) -o $@ $^

aesdatagen: $(aesdatagen_OBJECTS)
	$(CC) $(LDFLAGS) $(aesdatagen_LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

aescrypt.o: aescrypt.c aes.h debug.h
aes.o: aes.c aes.h debug.h
aes.o: debug.c debug.h
aes_data_gen.o: aes_data_gen.c

clean:
	$(foreach TRGT, $(TARGETS), $(RM) $($(TRGT)_OBJECTS) $(TRGT))

install: $(TARGET)
	$(INSTALL) $(TARGET) $(BINDIR)
