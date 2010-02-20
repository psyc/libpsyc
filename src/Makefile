CC=clang

dietlib:
	/opt/diet/bin/diet ${CC}  -c -Os parser.c -o libpsyc.o -DDEBUG	
	/opt/diet/bin/diet ar rcs libpsyc.a libpsyc.o

lib:
	${CC} -static -c -Os parser.c -lc -o libpsyc.o -DDEBUG	
	ar rcs libpsyc.a libpsyc.o



