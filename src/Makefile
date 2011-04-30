CFLAGS=-I../include -DDEBUG=2 -DPSYC_COMPILE_LIBRARY -g -O0 -Wall
CC=cc
# CC=clang

test -z "$DIET" || DIET=diet

S=packet.c misc.c parser.c match.c render.c memmem.c itoa.c variable.c
O=packet.o misc.o parser.o match.o render.o memmem.o itoa.o variable.o

default: 
	@/bin/echo -e "Usage:\n\tmake diet - compile with diet libc\n\tmake lib  - compile with normal gnu libc"

diet:
	${DIET} ${CC} -static -c -Os $S ${CFLAGS}
	@test -d ../lib || mkdir ../lib
	${DIET} ar rcs ../lib/libpsyc.a $O

glibc: $S
	${CC} -static -c -g -O0 $S -lc ${CFLAGS}
	@test -d ../lib || mkdir ../lib
	ar rcs ../lib/libpsyc.a $O

match: match.c
	${CC} -o $@ -DDEBUG=4 -DCMDTOOL -DTEST $<

itoa: itoa.c
	${CC} -o $@ -DDEBUG=4 -DCMDTOOL -DTEST -O0 $<

it: match

clean:
	rm -f $O ../lib/libpsyc.a
