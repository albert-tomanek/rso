CC=gcc
CFLAGS=-I.
DEPS=src/rso_io.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

rso: src/rso.o src/rso_io.o 
	gcc src/rso.o src/rso_io.o -L /usr/lib/i386-linux-gnu/ -lsndfile -o rso

install:
	cp rso /usr/bin