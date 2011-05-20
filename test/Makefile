OPT = -O2
DEBUG = 2
CFLAGS = -I../include -I../src -Wall -std=c99 ${OPT}
LDFLAGS = -L../lib
LOADLIBES = -lpsyc -lm
TARGETS = testPsyc testPsycSpeed testParser testMatch testRender testText isRoutingVar getVarType
O = test.o
WRAPPER =
DIET = diet
PORT = 4440
NC = nc
DIFF = diff
TEE = tee

ifeq ($(shell uname),SunOS)
  LOADLIBES_NET := -lsocket -lnsl
  DIFF = gdiff
endif

all: ${TARGETS}
it: all

testPsyc: LOADLIBES := ${LOADLIBES} ${LOADLIBES_NET}
testPsycSpeed: LOADLIBES := ${LOADLIBES} ${LOADLIBES_NET}
#testPsycSpeed: LOADLIBES := ${LOADLIBES_NET}

testJson: LOADLIBES := ${LOADLIBES_NET} -ljson

testJsonGlib: CFLAGS := ${CFLAGS} -I/usr/include/json-glib-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include
testJsonGlib: LOADLIBES := ${LOADLIBES_NET} -ljson-glib-1.0

testStrlen: LOADLIBES := ${LOADLIBES_NET}

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

bench: bench-psyc bench-json bench-xml

bench-dir:
	@mkdir -p ../bench/results

bench-psyc: bench-dir testStrlen testPsycSpeed
	for f in ../bench/packets/*.psyc; do bf=`basename $$f`; echo strlen: $$bf; ./testStrlen -sc 1000000 -f $$f | ${TEE} -a ../bench/results/$$bf.strlen; done
	for f in ../bench/packets/*.psyc; do bf=`basename $$f`; echo libpsyc: $$f; ./testPsycSpeed -sc 1000000 -f $$f | ${TEE} -a ../bench/results/$$bf; done

bench-json: bench-dir testStrlen testJson testJsonGlib
	for f in ../bench/packets/*.json; do bf=`basename $$f`; echo strlen: $$bf; ./testStrlen -sc 1000000 -f $$f | ${TEE} -a ../bench/results/$$bf.strlen; done
	for f in ../bench/packets/*.json; do bf=`basename $$f`; echo json-c: $$bf; ./testJson -snc 1000000 -f $$f | ${TEE} -a ../bench/results/$$bf; done
	for f in ../bench/packets/*.json; do bf=`basename $$f`; echo json-glib: $$bf; ./testJsonGlib -snc 1000000 -f $$f | ${TEE} -a ../bench/results/$$bf-glib; done

bench-xml: bench-dir testStrlen
	for f in ../bench/packets/*.xml; do bf=`basename $$f`; echo strlen: $$bf; ./testStrlen -sc 1000000 -f $$f | ${TEE} -a ../bench/results/$$bf.strlen; done

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
