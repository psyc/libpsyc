CC=cc
# CC=clang

default: 
	@/bin/echo -e "Usage:\n\tmake diet - compile with diet libc\n\tmake lib  - compile with normal gnu libc"

diet:
	/opt/diet/bin/diet ${CC} -static -c -Os parser.c -o libpsyc.o -DDEBUG	
	/opt/diet/bin/diet ar rcs libpsyc.a libpsyc.o

lib:
	${CC} -static -c -Os parser.c -lc -o libpsyc.o -DDEBUG	-I../include
	ar rcs libpsyc.a libpsyc.o

match: match.c
	${CC} -o $@ -DTEST $<

it: match
