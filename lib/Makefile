libdir = ${PREFIX}/lib

INSTALL = install
LIBS = libpsyc.a libpsyc.so

install: ${LIBS}

libdir:
	${INSTALL} -d ${libdir}

${LIBS}: libdir
	-${INSTALL} $@ ${libdir}

clean:
	rm -f ${LIBS}
