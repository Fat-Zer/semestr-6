CC = gcc
CFLAGS = -Wall -g -DAES_TABLE_ENCRIPTION
# CFLAGS = -Wall -O2

aestest_LDFLAGS = -lcrypto
aestest_OBJECTS = aestest.o aes.o debug.o
aescrypt_OBJECTS = aescrypt.o aes.o

aesdatagen_OBJECTS = aes_data_gen.o

TARGETS = aestest aesdatagen aescrypt

RM = rm -f
INSTALL = /usr/bin/install -c

BINDIR = /usr/local/bin

all: $(TARGETS)

aescrypt: $(aescrypt_OBJECTS)
	$(CC) $(LDFLAGS) $(aestest_LDFLAGS) -o $@ $^

aestest: $(aestest_OBJECTS)
	$(CC) $(LDFLAGS) $(aestest_LDFLAGS) -o $@ $^

aesdatagen: $(aesdatagen_OBJECTS)
	$(CC) $(LDFLAGS) $(aesdatagen_LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

aestest.o: aescrypt.c aes.h debug.h
aes.o: aes.c aes.h debug.h
aes.o: debug.c debug.h
aes_data_gen.o: aes_data_gen.c

clean:
	$(foreach TRGT, $(TARGETS), $(RM) $($(TRGT)_OBJECTS) $(TRGT))

install: $(TARGET)
	$(INSTALL) $(TARGET) $(BINDIR)
