.PHONY: doc test bench
.NOTPARALLEL: clean

indent_args = -nbad -bap -bbo -nbc -br -brs -ncdb -cdw -ce -ci4 -cli0 -cs -d0 -di1 \
-nfc1 -nfca -hnl -i4 -ip0 -l80 -lp -npcs -nprs -npsl -saf -sai -saw -nsc -nsob -nss

all:
	${MAKE} -C src

it: all

destdir:
	@if test "foo$(DESTDIR)" = "foo" ; then \
            echo "libpsyc install: You must provide a DESTDIR=/usr or such." ;\
	    exit 2 ;\
	else true; fi
	-@if [ ! -w "$(DESTDIR)" ]; then \
	    mkdir "$(DESTDIR)" ;\
	fi
	@if [ ! -w "$(DESTDIR)" ]; then \
            echo "libpsyc install: You must provide a writable DESTDIR." ;\
	    exit 2 ;\
	else true; fi

install: install-lib install-inc install-d

install-lib: destdir all
	${MAKE} -C lib install

install-inc: destdir all
	${MAKE} -C include install

install-d: destdir all
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

check: test

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

