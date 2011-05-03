OPT = -O2
DEBUG = 2
CFLAGS = -I../include -Wall ${OPT}
DIET = diet

S = packet.c misc.c parser.c match.c render.c memmem.c itoa.c variable.c text.c
O = packet.o misc.o parser.o match.o render.o memmem.o itoa.o variable.o text.o

all: CC := ${WRAPPER} ${CC}
all: lib

debug: CFLAGS += -DDEBUG=${DEBUG} -g
debug: CFLAGS := $(subst ${OPT},-O0,${CFLAGS})
debug: lib

diet: WRAPPER = ${DIET}
diet: CC := ${WRAPPER} ${CC}
diet: lib

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

help:
	@/bin/echo -e "Usage:\n\tmake - compile\n\tmake diet - compile with diet libc"
