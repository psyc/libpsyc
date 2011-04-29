CFLAGS=-I../include -DDEBUG -g -O0 -Wall
LDFLAGS=-L../src
LOADLIBES=-lpsyc -lm
TARGETS=testServer testParser testMatch testRender isRoutingVar
PORT=4440

all: $(TARGETS)
	./testRender
	./testMatch
	./isRoutingVar

test: $(TARGETS)
	for f in packets/full-* packets/error-*; do echo ">> $$f"; ./testParser $$f; done

nettest:
	for f in packets/full-*; do echo ">> $$f"; cat $$f | nc localhost $(PORT) | diff -u $$f -; done

nettesterr:
	for f in packets/error-*; do echo ">> $$f"; cat $$f | nc localhost $(PORT); done

nettestp1:
	(for f in packets/part-1-p*; do cat $$f; done) | nc localhost $(PORT) | diff -u packets/full-1 -

nettestp2:
	(for f in packets/part-1-length-p*; do cat $$f; done) | nc localhost $(PORT) | diff -u packets/full-1-length -

clean:
	rm -f $(TARGETS)

it: all
