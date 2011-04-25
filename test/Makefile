CFLAGS=-I../include -DDEBUG -g -O0 -Wall
LDFLAGS=-L../src
LOADLIBES=-lpsyc -lm
TARGETS=testParser testMatch testRender isRoutingVar

all: $(TARGETS)
	./testRender
	./testMatch
	./isRoutingVar

test: $(TARGETS)
	for f in packets/*; do echo "\n>> $$f"; ./testParser $$f; done

clean:
	rm -f $(TARGETS)

it: all
