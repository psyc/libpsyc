.PHONY: doc
.PHONY: tests

default:
	@/bin/echo -e "Usage:\n\tmake diet - compile with diet libc\n\tmake lib  - compile with normal gnu libc\n\tmake tests\n\tmake doc"

lib:
	make -C src lib

diet:
	make -C src diet

tests:
	make -C tests

doc:
	doxygen

clean:
	make -C src clean
	make -C tests clean
	rm -rf doc
