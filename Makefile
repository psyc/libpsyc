.PHONY: doc test bench

all:
	${MAKE} -C src

it: all

install: install-lib install-inc install-d

install-lib: all
	${MAKE} -C lib install

install-inc: all
	${MAKE} -C include install

install-d: all
	${MAKE} -C d install

debug:
	${MAKE} -C src debug

diet:
	${MAKE} -C src diet

debugtest: testdebug test

testdebug: debug
	${MAKE} -C test debug

test: all
	${MAKE} -C test test nettest

bench: all
	${MAKE} -C test bench

doc:
	doxygen

clean:
	${MAKE} -C src clean
	${MAKE} -C lib clean
	${MAKE} -C test clean
	rm -rf doc/html doc/latex doc/man

help:
	@/bin/echo -e "Usage:\n\tmake - compile\n\tmake diet - compile with diet libc\n\tmake test\n\tmake doc\n\tmake install [prefix=/usr]"
