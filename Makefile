ODIR=obj/
SDIR=src/
IDIR=include/
LIBS=
CLAGS=-std=c99 -pedantic -Wall -O2 -fPIC
LDLIBS=-ldl

CC=clang

all: kant libkant.so

kant: $(SRC)kant_linux.c $(SRC)kant.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(ODIR)$@ $< $(LDLIBS)

libkant.so: kant.c kant.h
	$(CC) $(CFLAGS) -shared $(LDFLAGS) -o $(ODIR)$@ $< $(LDLIBS)
