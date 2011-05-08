.PHONY: doc
.PHONY: test
.PHONY: lib

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
	${MAKE} -C test test
	${MAKE} -C test nettest
	${MAKE} -C test nettestr

doc:
	doxygen

clean:
	make -C src clean
	make -C test clean
	rm -rf doc/html doc/latex doc/man

help:
	@/bin/echo -e "Usage:\n\tmake - compile\n\tmake diet - compile with diet libc\n\tmake test\n\tmake doc"
