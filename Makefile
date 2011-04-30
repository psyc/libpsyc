.PHONY: doc
.PHONY: test
.PHONY: lib

all:
	${MAKE} -C src

diet:
	${MAKE} -C src diet

test:
	${MAKE} -C test

doc:
	doxygen

clean:
	make -C src clean
	make -C test clean
	rm -rf doc/html doc/latex doc/man

help:
	@/bin/echo -e "Usage:\n\tmake - compile\n\tmake diet - compile with diet libc\n\tmake test\n\tmake doc"
