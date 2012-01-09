OPT = -O2
DEBUG = 2
CFLAGS = -I../include -I../src -Wall -std=c99 ${OPT}
LDFLAGS = -L../lib
LOADLIBES = -lpsyc -lm
TARGETS = test_psyc test_psyc_speed test_parser test_match test_render test_text var_is_routing var_type uniform_parse test_list test_table test_packet_id method
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

test_psyc: LOADLIBES := ${LOADLIBES} ${LOADLIBES_NET}
test_psyc_speed: LOADLIBES := ${LOADLIBES} ${LOADLIBES_NET}
#test_psyc_speed: LOADLIBES := ${LOADLIBES_NET}

test_json: LOADLIBES := ${LOADLIBES_NET} -ljson

test_json_glib: CFLAGS := ${CFLAGS} -I/usr/include/json-glib-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include
test_json_glib: LOADLIBES := ${LOADLIBES_NET} -ljson-glib-1.0

test_strlen: LOADLIBES := ${LOADLIBES_NET}

diet: WRAPPER = ${DIET}
diet: all

debug: CFLAGS += -DDEBUG=${DEBUG} -g
debug: CFLAGS := $(subst ${OPT},-O0,${CFLAGS})
debug: all

clean:
	rm -f ${TARGETS} $O

test: ${TARGETS}
	./test_render
	./test_match
	./test_text
	./var_is_routing
	./var_type
	./uniform_parse
	./test_list
	./test_table
	./test_packet_id
	x=0; for f in packets/[0-9]*; do echo ">> $$f"; ./test_psyc -f $$f | ${DIFF} -u $$f -; x=$$((x+$$?)); done; exit $$x
	x=0; for f in packets/[0-9]*; do echo ">> $$f"; ./test_psyc -rf $$f | ${DIFF} -u $$f -; x=$$((x+$$?)); done; exit $$x

.NOTPARALLEL: nettestrun

nettest: nettestfull nettestsplit

nettestrun: srvstart pkt srvkill

nettestfull:
	${MAKE} nettestrun; x=$$?; pkill -x test_psyc; exit $$x
	${MAKE} nettestrun srv_args=-r; x=$$?; pkill -x test_psyc; exit $$x

split_max = 10
nettestsplit:
	x=0; for n in `seq 1 ${split_max}`; do ${MAKE} nettestrun srv_args="-b $$n" && ${MAKE} nettestrun srv_args="-r -b $$n" || break; done; x=$$?; pkill -x test_psyc; exit $$x

pkt:
	x=0; for f in packets/[0-9]*; do echo ">> $$f"; cat $$f | nc localhost ${PORT} | ${DIFF} -u $$f -; x=$$((x+$$?)); done; exit $$x

pktsplit:
	x=0; for f in packets/[0-9]*; do echo ">> $$f"; ./splittest.pl $$f ${PORT} | ${DIFF} -u $$f -; x=$$((x+$$?)); done; exit $$x

pkterr:
	for f in packets/err-*; do echo ">> $$f"; cat $$f | nc localhost ${PORT}; done

srvstart:
	pkill -x test_psyc; exit 0
	./test_psyc -p ${PORT} -S ${srv_args} &

srvkill:
	pkill -x test_psyc

bench: bench-genpkts bench-psyc bench-psyc-bin bench-json bench-json-bin bench-xml

bench-dir:
	@mkdir -p ../bench/results

bench-psyc: bench-dir test_strlen test_psyc_speed
	for f in ../bench/packets/*.psyc; do bf=`basename $$f`; echo strlen: $$bf; ./test_strlen -sc 1000000 -f $$f | ${TEE} -a ../bench/results/$$bf.strlen; done
	for f in ../bench/packets/*.psyc; do bf=`basename $$f`; echo libpsyc: $$f; ./test_psyc_speed -sc 1000000 -f $$f | ${TEE} -a ../bench/results/$$bf; done

bench-psyc-bin: bench-dir test_strlen test_psyc_speed
	for f in `ls ../bench/packets/binary/*.psyc | sort -r`; do bf=`basename $$f`; echo "libpsyc: $$f * 1000000"; ./test_psyc_speed -sc 1000000 -f $$f | ${TEE} -a ../bench/results/$$bf; done
	c=1000000; for f in `ls ../bench/packets/binary/*.psyc | sort -r`; do bf=`basename $$f`; echo "strlen: $$bf * $$c"; ./test_strlen -sc $$c -f $$f | ${TEE} -a ../bench/results/$$bf.strlen; c=$$((c/10)); done

bench-json: bench-dir test_json test_json_glib
#	for f in ../bench/packets/*.json; do bf=`basename $$f`; echo strlen: $$bf; ./test_strlen -sc 1000000 -f $$f | ${TEE} -a ../bench/results/$$bf.strlen; done
	for f in ../bench/packets/*.json; do bf=`basename $$f`; echo json-c: $$bf; ./test_json -snc 1000000 -f $$f | ${TEE} -a ../bench/results/$$bf; done
	for f in ../bench/packets/*.json; do bf=`basename $$f`; echo json-glib: $$bf; ./test_json_glib -snc 1000000 -f $$f | ${TEE} -a ../bench/results/$$bf-glib; done

bench-json-bin: bench-dir test_json test_json_glib
	c=1000000; for f in `ls ../bench/packets/binary/*.json | sort -r`; do bf=`basename $$f`; echo "json-c: $$bf * $$c"; ./test_json -snc $$c -f $$f | ${TEE} -a ../bench/results/$$bf; c=$$((c/10)); done
	c=1000000; for f in `ls ../bench/packets/binary/*.json | sort -r`; do bf=`basename $$f`; echo "json-glib: $$bf * $$c"; ./test_json_glib -snc $$c -f $$f | ${TEE} -a ../bench/results/$$bf-glib; c=$$((c/10)); done

bench-xml: bench-dir
	@[[ -n "${xmlbench}" ]] || (echo xmlbench path needs to be set with xmlbench=/path/to/xmlbench; exit 1)
#	for f in ../bench/packets/*.xml; do bf=`basename $$f`; echo strlen: $$bf; ./test_strlen -sc 1000000 -f $$f | ${TEE} -a ../bench/results/$$bf.strlen; done
	for f in ../bench/packets/*.xml; do bf=`basename $$f`; echo libxml: $$bf; ${xmlbench}/parse/libxml 1000000 $$f | ${TEE} -a ../bench/results/$$bf-libxml; done
	for f in ../bench/packets/*.xml; do bf=`basename $$f`; echo libxml-sax: $$bf; ${xmlbench}/parse/libxml-sax 1000000 $$f | ${TEE} -a ../bench/results/$$bf-libxml-sax; done
	for f in ../bench/packets/*.xml; do bf=`basename $$f`; echo rapidxml: $$bf; ${xmlbench}/parse/rapidxml 1000000 $$f | ${TEE} -a ../bench/results/$$bf-rapidxml; done

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
