includedir = ${PREFIX}/include

INSTALL = install
HEADERS = psyc.h

install: ${HEADERS}
	${MAKE} -C psyc install

installdir:
	${INSTALL} -d ${includedir}

${HEADERS}: installdir
	${INSTALL} -m 644 $@ ${includedir}
