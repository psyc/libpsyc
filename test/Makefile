OPT = -O2
DEBUG = 2
CFLAGS = -I../include -Wall ${OPT}
LDFLAGS = -L../lib
LOADLIBES = -lpsyc -lm
TARGETS = testServer testParser testMatch testRender testText isRoutingVar getVarType
WRAPPER =
DIET = diet
PORT = 4440

all: ${TARGETS}

diet: WRAPPER = ${DIET}
diet: all

debug: CFLAGS += -DDEBUG=${DEBUG} -g
debug: CFLAGS := $(subst ${OPT},-O0,${CFLAGS})
debug: all

test: ${TARGETS}
	./testRender
	./testMatch
	./isRoutingVar
	./getVarType
	for f in packets/full-*; do echo ">> $$f"; ./testParser $$f; done

netstart:
	./testServer ${PORT}

netstartr:
	./testServer ${PORT} -r

netstartv:
	./testServer ${PORT} -v

netstartrv:
	./testServer ${PORT} -r -v

nettest:
	for f in packets/full-*; do echo ">> $$f"; cat $$f | nc localhost ${PORT} | diff -u $$f -; done

nettesterr:
	for f in packets/error-*; do echo ">> $$f"; cat $$f | nc localhost ${PORT}; done

splittest:
	for f in packets/full-*; do echo ">> $$f"; ./splittest.pl $$f ${PORT} | diff -u $$f -; done

nettestp1:
	(for f in packets/part-1-p*; do cat $$f; done) | nc localhost ${PORT} | diff -u packets/full-1 -

nettestp2:
	(for f in packets/part-1-length-p*; do cat $$f; done) | nc localhost ${PORT} | diff -u packets/full-1-length -

clean:
	rm -f ${TARGETS}

it: all
