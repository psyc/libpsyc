OPT = -O2
DEBUG = 2
CFLAGS = -I../include -Wall -std=c99 ${OPT}
LDFLAGS = -L../lib
LOADLIBES = -lpsyc -lm
LOADLIBES_NET = ${LOADLIBES}
TARGETS = testServer testParser testMatch testRender testText isRoutingVar getVarType
WRAPPER =
DIET = diet
PORT = 4440
NC = nc
DIFF = diff

ifeq ($(shell uname),SunOS)
  LOADLIBES_NET := ${LOADLIBES_NET} -lsocket -lnsl
  DIFF = gdiff
endif

all: ${TARGETS}
it: all

testServer: LOADLIBES := ${LOADLIBES_NET}

diet: WRAPPER = ${DIET}
diet: all

debug: CFLAGS += -DDEBUG=${DEBUG} -g
debug: CFLAGS := $(subst ${OPT},-O0,${CFLAGS})
debug: all

clean:
	rm -f ${TARGETS}

test: ${TARGETS}
	./testRender
	./testMatch
	./testText
	./isRoutingVar
	./getVarType
	for f in packets/[0-9]*; do echo ">> $$f"; ./testParser $$f; done
	for f in packets/[0-9]*; do echo ">> $$f"; ./testParser $$f -r; done

# test packet parsing & rendering with the test server
nettest: srvstart pkt pktsplit srvkill

# same test but parse routing headers only
nettestr: srvstartr pkt pktsplit srvkill

pkt:
	for f in packets/[0-9]*; do echo ">> $$f"; cat $$f | nc localhost ${PORT} | ${DIFF} -u $$f -; done

pktsplit:
	for f in packets/[0-9]*; do echo ">> $$f"; ./splittest.pl $$f ${PORT} | ${DIFF} -u $$f -; done

pkterr:
	for f in packets/err-*; do echo ">> $$f"; cat $$f | nc localhost ${PORT}; done

srvstart:
	./testServer ${PORT} &

srvstartr:
	./testServer ${PORT} -r &

srvkill:
	pkill -x testServer
