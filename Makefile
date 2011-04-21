.PHONY: doc
.PHONY: test

default:
	@/bin/echo -e "Usage:\n\tmake diet - compile with diet libc\n\tmake lib  - compile with normal gnu libc\n\tmake test\n\tmake doc"

lib:
	make -C src lib

diet:
	make -C src diet

test:
	make -C test

doc:
	doxygen

clean:
	make -C src clean
	make -C test clean
	rm -rf doc/html doc/latex doc/man
