CC = gcc
CFLAGS = -Wall -g
# CFLAGS = -Wall -O2
LDFLAGS = -lssl

OBJECTS = aescrypt.o aes.o debug.o
TARGET = aescrypt

RM = rm -f
INSTALL = /usr/bin/install -c

BINDIR = /usr/local/bin

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

aescrypt.o: aescrypt.c aes.h debug.h
aes.o: aes.c aes.h debug.h
aes.o: debug.c debug.h

clean:
	$(RM) $(OBJECTS) $(TARGET)

install: $(TARGET)
	$(INSTALL) $(TARGET) $(BINDIR)
