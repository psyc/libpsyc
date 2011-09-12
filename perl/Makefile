CFLAGS = -I../include -Wall -std=c99 -fPIC
SWIG = swig
PERL = perl

I = psyc.i
S = psyc_wrap.c
O = psyc_wrap.o
SO = PSYC.so
PM = PSYC.pm
PSYCO = ../src/packet.o ../src/parse.o ../src/match.o ../src/render.o ../src/memmem.o ../src/itoa.o ../src/variable.o ../src/text.o

all: swig lib

swig:
	${SWIG} -I../include -perl5 $I

CFLAGS = ${CFLAGS_COMMON} $(shell ${PERL} -MConfig -e 'print join(" ", @Config{qw(ccflags optimize cccdlflags)}, "-I$$Config{archlib}/CORE")') -D_XOPEN_SOURCE
$O: $S

lib: $O
	${MAKE} -C ../src ${PSYCO}
	${CC}	${CFLAGS} $(shell ${PERL} -MConfig -e 'print $$Config{lddlflags}') ${PSYCO} $O -o ${SO}

clean:
	rm -f $S $O ${SO} ${PM}
