CFLAGS=-I../include -DDEBUG=2 -DPSYC_COMPILE_LIBRARY -g -O0 -Wall
CC=cc
# CC=clang

S=psyc.c parser.c match.c render.c memmem.c itoa.c variable.c
O=psyc.o parser.o match.o render.o memmem.o itoa.o variable.o

default: 
	@/bin/echo -e "Usage:\n\tmake diet - compile with diet libc\n\tmake lib  - compile with normal gnu libc"

diet:
	diet ${CC} -static -c -Os $S ${CFLAGS}
	diet ar rcs libpsyc.a $O

lib: $S
	${CC} -static -c -g -O0 $S -lc ${CFLAGS}
	ar rcs libpsyc.a $O

match: match.c
	${CC} -o $@ -DDEBUG=4 -DCMDTOOL -DTEST $<

itoa: itoa.c
	${CC} -o $@ -DDEBUG=4 -DCMDTOOL -DTEST -O0 $<

it: match

clean:
	rm -f $O
