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
	x=0; for f in packets/[0-9]*; do echo ">> $$f"; ./testParser $$f; x=$$((x+$$?)); done; exit $$x
	x=0; for f in packets/[0-9]*; do echo ">> $$f"; ./testParser $$f -r; x=$$((x+$$?)); done; exit $$x

nettest: nettestfull nettestsplit

nettestrun: srvstart pkt srvkill

nettestfull:
	${MAKE} nettestrun; x=$$?; pkill -x testServer; exit $$x
	${MAKE} nettestrun srv_args=r; x=$$?; pkill -x testServer; exit $$x

split_max = 10
nettestsplit:
	x=0; for n in `seq 1 ${split_max}`; do ${MAKE} nettestrun srv_recvbuf=$$n && ${MAKE} nettestrun srv_args=r srv_recvbuf=$$n || break; done; x=$$?; pkill -x testServer; exit $$x

pkt:
	x=0; for f in packets/[0-9]*; do echo ">> $$f"; cat $$f | nc localhost ${PORT} | ${DIFF} -u $$f -; x=$$((x+$$?)); done; exit $$x

pktsplit:
	x=0; for f in packets/[0-9]*; do echo ">> $$f"; ./splittest.pl $$f ${PORT} | ${DIFF} -u $$f -; x=$$((x+$$?)); done; exit $$x

pkterr:
	for f in packets/err-*; do echo ">> $$f"; cat $$f | nc localhost ${PORT}; done

srvstart:
	pkill -x testServer; exit 0
	./testServer ${PORT} -${srv_args} ${srv_recvbuf} &

srvkill:
	pkill -x testServer
