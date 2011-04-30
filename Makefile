.PHONY: doc
.PHONY: test
.PHONY: lib

default:
	@/bin/echo -e "Usage:\n\tmake diet - compile with diet libc\n\tmake glibc  - compile with normal gnu libc\n\tmake test\n\tmake doc"

lib:
	${MAKE} -C src lib

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
