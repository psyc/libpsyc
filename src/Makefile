OPT = -O2
DEBUG = 2
CFLAGS = -I../include -Wall ${OPT}
DIET = diet
WRAPPER =

S = packet.c misc.c parser.c match.c render.c memmem.c itoa.c variable.c
O = packet.o misc.o parser.o match.o render.o memmem.o itoa.o variable.o

all: lib

help:
	@/bin/echo -e "Usage:\n\tmake diet - compile with diet libc\n\tmake lib  - compile with normal gnu libc"

debug: CFLAGS += -DDEBUG=${DEBUG} -g
debug: CFLAGS := $(subst ${OPT},-O0,${CFLAGS})
debug: lib

diet: WRAPPER = ${DIET}
diet: lib

lib: CC := ${WRAPPER} ${CC}
lib: $O
	@mkdir -p ../lib
	${WRAPPER} ar rcs ../lib/libpsyc.a $O

match: match.c
	${CC} -o $@ -DDEBUG=4 -DCMDTOOL -DTEST $<

itoa: itoa.c
	${CC} -o $@ -DDEBUG=4 -DCMDTOOL -DTEST -O0 $<

it: match

clean:
	rm -f $O ../lib/libpsyc.a
