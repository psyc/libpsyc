prefix = /usr
includedir = ${prefix}/include/d

INSTALL = install
HEADERS = common.d parse.d

install: ${HEADERS}

installdir:
	${INSTALL} -d ${includedir}/psyc

${HEADERS}: installdir
	${INSTALL} -m 644 $@ ${includedir}/psyc
