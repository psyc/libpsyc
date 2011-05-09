prefix = /usr
includedir = ${prefix}/include

INSTALL = install
HEADERS = packet.h parser.h render.h syntax.h text.h

install: ${HEADERS}

installdir:
	${INSTALL} -d ${includedir}/psyc

${HEADERS}: installdir
	${INSTALL} -m 644 $@ ${includedir}/psyc
