# Why do we have a Makefile in the include directory?

includedir = ${DESTDIR}/include

INSTALL = install
HEADERS = match.h method.h packet.h parse.h render.h text.h uniform.h variable.h

install: ${HEADERS}

installdir:
	${INSTALL} -d ${includedir}/psyc

${HEADERS}: installdir
	${INSTALL} -m 644 $@ ${includedir}/psyc
