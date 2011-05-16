OPT = -O2
DEBUG = 2
CFLAGS_COMMON = -Wall -std=c99 ${OPT}
CFLAGS = -I../include -I../src ${CFLAGS_COMMON}
LDFLAGS = -L../lib
LOADLIBES = -lpsyc -lm
TARGETS = testPsyc testPsycFast testParser testMatch testRender testText isRoutingVar getVarType
O = test.o
WRAPPER =
DIET = diet
PORT = 4440
NC = nc
DIFF = diff

ifeq ($(shell uname),SunOS)
  LOADLIBES_NET := -lsocket -lnsl
  DIFF = gdiff
endif

all: ${TARGETS}
it: all

testPsyc: LOADLIBES := ${LOADLIBES} ${LOADLIBES_NET}
testPsycFast: LOADLIBES := ${LOADLIBES} ${LOADLIBES_NET}
#testPsycFast: LOADLIBES := ${LOADLIBES_NET}

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
	x=0; for f in packets/[0-9]*; do echo ">> $$f"; ./testPsyc -f $$f | ${DIFF} -u $$f -; x=$$((x+$$?)); done; exit $$x
	x=0; for f in packets/[0-9]*; do echo ">> $$f"; ./testPsyc -rf $$f | ${DIFF} -u $$f -; x=$$((x+$$?)); done; exit $$x

.NOTPARALLEL: nettestrun

nettest: nettestfull nettestsplit

nettestrun: srvstart pkt srvkill

nettestfull:
	${MAKE} nettestrun; x=$$?; pkill -x testPsyc; exit $$x
	${MAKE} nettestrun srv_args=-r; x=$$?; pkill -x testPsyc; exit $$x

split_max = 10
nettestsplit:
	x=0; for n in `seq 1 ${split_max}`; do ${MAKE} nettestrun srv_args="-b $$n" && ${MAKE} nettestrun srv_args="-r -b $$n" || break; done; x=$$?; pkill -x testPsyc; exit $$x

pkt:
	x=0; for f in packets/[0-9]*; do echo ">> $$f"; cat $$f | nc localhost ${PORT} | ${DIFF} -u $$f -; x=$$((x+$$?)); done; exit $$x

pktsplit:
	x=0; for f in packets/[0-9]*; do echo ">> $$f"; ./splittest.pl $$f ${PORT} | ${DIFF} -u $$f -; x=$$((x+$$?)); done; exit $$x

pkterr:
	for f in packets/err-*; do echo ">> $$f"; cat $$f | nc localhost ${PORT}; done

srvstart:
	pkill -x testPsyc; exit 0
	./testPsyc -p ${PORT} -S ${srv_args} &

srvkill:
	pkill -x testPsyc
