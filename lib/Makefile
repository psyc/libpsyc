prefix = /usr
libdir = ${prefix}/lib

INSTALL = install
LIBS = libpsyc.a

install: ${LIBS}

libdir:
	${INSTALL} -d ${libdir}

${LIBS}: libdir
	${INSTALL} $@ ${libdir}
