CC=gcc
CFLAGS=-I.
DEPS=rso_io.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

rso: rso.o rso_io.o 
	gcc rso.o rso_io.o -L /usr/lib/i386-linux-gnu/ -lsndfile -o rso
