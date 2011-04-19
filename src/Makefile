CFLAGS=-I../../include -DDEBUG -DPSYC_COMPILE_LIBRARY -g -O0 -Wall
CC=cc -I../include
# CC=clang

S=parser.c match.c
O=parser.o match.o

default: 
	@/bin/echo -e "Usage:\n\tmake diet - compile with diet libc\n\tmake lib  - compile with normal gnu libc"

diet:
	diet ${CC} -static -c -Os $S ${CFLAGS}
	diet ar rcs libpsyc.a $O

lib: $S
	${CC} -static -c -g -O0 $S -lc ${CFLAGS}
	ar rcs libpsyc.a $O

match: match.c
	${CC} -o $@ -DDEBUG=2 -DCMDTOOL -DTEST $<

it: match

clean:
	rm -f $O
