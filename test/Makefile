OPT = -O2
DEBUG = 2
CFLAGS = -I../include -I../src -Wall -std=c99 ${OPT}
LDFLAGS = -L../lib
LOADLIBES = -lpsyc -lm
LOADLIBES_NET = ${LOADLIBES}
TARGETS = testServerPsyc testParser testMatch testRender testText isRoutingVar getVarType
O = testServer.o
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

testServerPsyc: LOADLIBES := ${LOADLIBES_NET}
testServerPsyc: testServer.o

diet: WRAPPER = ${DIET}
diet: all

debug: CFLAGS += -DDEBUG=${DEBUG} -g
debug: CFLAGS := $(subst ${OPT},-O0,${CFLAGS})
debug: all

clean:
	rm -f ${TARGETS} $O

test: ${TARGETS}
	./testRender
	./testMatch
	./testText
	./isRoutingVar
	./getVarType
	x=0; for f in packets/[0-9]*; do echo ">> $$f"; ./testParser $$f; x=$$((x+$$?)); done; exit $$x
	x=0; for f in packets/[0-9]*; do echo ">> $$f"; ./testParser $$f -r; x=$$((x+$$?)); done; exit $$x

.NOTPARALLEL: nettestrun

nettest: nettestfull nettestsplit

nettestrun: srvstart pkt srvkill

nettestfull:
	${MAKE} nettestrun; x=$$?; pkill -x testServerPsyc; exit $$x
	${MAKE} nettestrun srv_args=r; x=$$?; pkill -x testServerPsyc; exit $$x

split_max = 10
nettestsplit:
	x=0; for n in `seq 1 ${split_max}`; do ${MAKE} nettestrun srv_recvbuf=$$n && ${MAKE} nettestrun srv_args=r srv_recvbuf=$$n || break; done; x=$$?; pkill -x testServerPsyc; exit $$x

pkt:
	x=0; for f in packets/[0-9]*; do echo ">> $$f"; cat $$f | nc localhost ${PORT} | ${DIFF} -u $$f -; x=$$((x+$$?)); done; exit $$x

pktsplit:
	x=0; for f in packets/[0-9]*; do echo ">> $$f"; ./splittest.pl $$f ${PORT} | ${DIFF} -u $$f -; x=$$((x+$$?)); done; exit $$x

pkterr:
	for f in packets/err-*; do echo ">> $$f"; cat $$f | nc localhost ${PORT}; done

srvstart:
	pkill -x testServerPsyc; exit 0
	./testServerPsyc ${PORT} -${srv_args} ${srv_recvbuf} &

srvkill:
	pkill -x testServerPsyc
