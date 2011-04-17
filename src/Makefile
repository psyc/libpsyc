CC=cc -I../include
# CC=clang

S=parser.c match.c
O=parser.o match.o

default: 
	@/bin/echo -e "Usage:\n\tmake diet - compile with diet libc\n\tmake lib  - compile with normal gnu libc"

diet:
	diet ${CC} -static -c -Os $S -DDEBUG -DPSYC_COMPILE_LIBRARY
	diet ar rcs libpsyc.a $O

lib: $S
	${CC} -static -c -g -O0 $S -lc -DDEBUG -DPSYC_COMPILE_LIBRARY
	ar rcs libpsyc.a $O

match: match.c
	${CC} -o $@ -DDEBUG=2 -DCMDTOOL -DTEST $<

it: match
