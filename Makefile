CC=gcc
CFLAGS=-Wall
LINKERFLAG=-lncurses -lm

all: ising.c
	$(CC) $(CFLAGS) ising.c -o ising $(LINKERFLAG)

dbg: ising.c
	$(CC) -g $(CFLAGS) ising.c -o ising $(LINKERFLAG)

clean:
	rm -rf ising
