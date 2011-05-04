OPT = -O2
DEBUG = 2
CFLAGS = -I../include -Wall -std=c99 ${OPT}
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
	./testText
	./isRoutingVar
	./getVarType
	for f in packets/[0-9]*; do echo ">> $$f"; ./testParser $$f; done
	for f in packets/[0-9]*; do echo ">> $$f"; ./testParser $$f -r; done

testServer: CFLAGS := $(subst -std=c99,,${CFLAGS})

netstart:
	./testServer ${PORT}

netstartr:
	./testServer ${PORT} -r

netstartv:
	./testServer ${PORT} -v

netstartrv:
	./testServer ${PORT} -rv

nettest: nettestfull nettestsplit

nettestfull:
	for f in packets/[0-9]*; do echo ">> $$f"; cat $$f | nc localhost ${PORT} | diff -u $$f -; done

nettestsplit:
	for f in packets/[0-9]*; do echo ">> $$f"; ./splittest.pl $$f ${PORT} | diff -u $$f -; done

nettesterr:
	for f in packets/err-*; do echo ">> $$f"; cat $$f | nc localhost ${PORT}; done

clean:
	rm -f ${TARGETS}

it: all
