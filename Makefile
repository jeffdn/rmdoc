# rmdoc

CC = gcc
CFLAGS = -g -pipe -O2 -W -Wall -pedantic

all: rmdoc

rmdoc.o: src/rmdoc.c
	${CC} ${CFLAGS} -c src/rmdoc.c

rmdoc: rmdoc.o
	${CC} ${CFLAGS} rmdoc.o -o rmdoc

clean:
	rm -f *.o rmdoc

