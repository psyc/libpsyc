CFLAGS = -Wall -std=c99 -fPIC -I../include $(shell ${PYTHON_CONFIG} --includes)
SWIG = swig
PYTHON_CONFIG = python-config

I = psyc.i
S = psyc_wrap.c
O = psyc_wrap.o
SO = _PSYC.so
PY = PSYC.py
PSYCO = ../src/packet.o ../src/parse.o ../src/match.o ../src/render.o ../src/memmem.o ../src/itoa.o ../src/variable.o ../src/text.o

all: swig lib

swig:
	${SWIG} -I../include -python $I

$O: $S

lib: $O
	${MAKE} -C ../src ${PSYCO}
	${CC}	-shared ${PSYCO} $O -o ${SO}

clean:
	rm -f $S $O ${SO} ${PY} ${PY}c
