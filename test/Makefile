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


bench: bench-genpkts bench-psyc bench-json bench-xml

bench-dir:
	@mkdir -p ../bench/results

bench-psyc: bench-dir testStrlen testPsycSpeed
	for f in ../bench/packets/*.psyc `ls ../bench/packets/binary/*.psyc | sort -r`; do bf=`basename $$f`; echo strlen: $$bf; ./testStrlen -sc 1000000 -f $$f | ${TEE} -a ../bench/results/$$bf.strlen; done
	for f in ../bench/packets/*.psyc `ls ../bench/packets/binary/*.psyc | sort -r`; do bf=`basename $$f`; echo libpsyc: $$f; ./testPsycSpeed -sc 1000000 -f $$f | ${TEE} -a ../bench/results/$$bf; done

bench-json: bench-dir testStrlen testJson testJsonGlib
#	for f in ../bench/packets/*.json `ls ../bench/packets/binary/*.json | sort -r`; do bf=`basename $$f`; echo strlen: $$bf; ./testStrlen -sc 1000000 -f $$f | ${TEE} -a ../bench/results/$$bf.strlen; done
	for f in ../bench/packets/*.json `ls ../bench/packets/binary/*.json | sort -r`; do bf=`basename $$f`; echo json-c: $$bf; ./testJson -snc 1000000 -f $$f | ${TEE} -a ../bench/results/$$bf; done
	for f in ../bench/packets/*.json; do bf=`basename $$f`; echo json-glib: $$bf; ./testJsonGlib -snc 1000000 -f $$f | ${TEE} -a ../bench/results/$$bf-glib; done

bench-xml: bench-dir testStrlen
#	for f in ../bench/packets/*.xml `ls ../bench/packets/binary/*.xml | sort -r`; do bf=`basename $$f`; echo strlen: $$bf; ./testStrlen -sc 1000000 -f $$f | ${TEE} -a ../bench/results/$$bf.strlen; done

bench-genpkts:
	@${MAKE} genpkt header=../bench/packets/binary/psyc-header content=../bench/packets/binary/psyc-content bs=7000 of=../bench/packets/binary/7K.psyc
	@${MAKE} genpkt header=../bench/packets/binary/psyc-header content=../bench/packets/binary/psyc-content bs=70000 of=../bench/packets/binary/70K.psyc
	@${MAKE} genpkt header=../bench/packets/binary/psyc-header content=../bench/packets/binary/psyc-content bs=700000 of=../bench/packets/binary/700K.psyc
	@${MAKE} genpkt header=../bench/packets/binary/psyc-header content=../bench/packets/binary/psyc-content bs=7000000 of=../bench/packets/binary/7000K.psyc
	@${MAKE} genpkt header=../bench/packets/binary/psyc-header content=../bench/packets/binary/psyc-content bs=7000000 count=10 of=../bench/packets/binary/70000K.psyc
#	@${MAKE} genpkt header=../bench/packets/binary/psyc-header content=../bench/packets/binary/psyc-content bs=7000000 count=100 of=../bench/packets/binary/700000K.psyc

	@${MAKE} genb64 header=../bench/packets/binary/json-header footer=../bench/packets/binary/json-footer bs=7000 of=../bench/packets/binary/7K.json
	@${MAKE} genb64 header=../bench/packets/binary/json-header footer=../bench/packets/binary/json-footer bs=70000 of=../bench/packets/binary/70K.json
	@${MAKE} genb64 header=../bench/packets/binary/json-header footer=../bench/packets/binary/json-footer bs=700000 of=../bench/packets/binary/700K.json
	@${MAKE} genb64 header=../bench/packets/binary/json-header footer=../bench/packets/binary/json-footer bs=7000000 of=../bench/packets/binary/7000K.json
	@${MAKE} genb64 header=../bench/packets/binary/json-header footer=../bench/packets/binary/json-footer bs=7000000 count=10 of=../bench/packets/binary/70000K.json
#	@${MAKE} genb64 header=../bench/packets/binary/json-header footer=../bench/packets/binary/json-footer bs=7000000 count=100 of=../bench/packets/binary/700000K.json

	@${MAKE} genb64 header=../bench/packets/binary/xml-header footer=../bench/packets/binary/xml-footer bs=7000 of=../bench/packets/binary/7K.xml
	@${MAKE} genb64 header=../bench/packets/binary/xml-header footer=../bench/packets/binary/xml-footer bs=70000 of=../bench/packets/binary/70K.xml
	@${MAKE} genb64 header=../bench/packets/binary/xml-header footer=../bench/packets/binary/xml-footer bs=700000 of=../bench/packets/binary/700K.xml
	@${MAKE} genb64 header=../bench/packets/binary/xml-header footer=../bench/packets/binary/xml-footer bs=7000000 of=../bench/packets/binary/7000K.xml
	@${MAKE} genb64 header=../bench/packets/binary/xml-header footer=../bench/packets/binary/xml-footer bs=7000000 count=10 of=../bench/packets/binary/70000K.xml
#	@${MAKE} genb64 header=../bench/packets/binary/xml-header footer=../bench/packets/binary/xml-footer bs=7000000 count=100 of=../bench/packets/binary/700000K.xml

bs = 1
count = 1
genpkt:
	@[[ -n "${of}" ]]
	[[ -f "${of}" ]] || ( \
		cat ${header} >${of}; \
		perl -le 'my @se=stat(q('${content}')); print $$se[7] + ${bs} * ${count} + 1' >>${of}; \
		cat ${content} >>${of}; \
		dd if=/dev/urandom of=${of} bs=${bs} count=${count} oflag=append conv=notrunc; \
		echo -ne "\n|\n" >>${of} )

bs = 1
count = 1
genb64:
	@[[ -n "${of}" ]]
	[[ -f "${of}" ]] || (cat ${header} >${of}; dd if=/dev/urandom bs=${bs} count=${count} | base64 -w0 >>${of}; cat ${footer} >>${of})
