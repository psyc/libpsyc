.PHONY: doc test

all:
	${MAKE} -C src

debug:
	${MAKE} -C src debug

diet:
	${MAKE} -C src diet

debugtest: testdebug test

testdebug: debug
	${MAKE} -C test debug

test: all
	${MAKE} -C test test nettest

doc:
	doxygen

clean:
	${MAKE} -C src clean
	${MAKE} -C test clean
	rm -rf doc/html doc/latex doc/man

help:
	@/bin/echo -e "Usage:\n\tmake - compile\n\tmake diet - compile with diet libc\n\tmake test\n\tmake doc"
