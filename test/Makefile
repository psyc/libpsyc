CFLAGS=-I../include -DDEBUG -g -O0 -Wall
LDFLAGS=-L../src
LOADLIBES=-lpsyc
TARGETS=testParser testMatch testRender

all: $(TARGETS)

test:
	for f in packets/*; do echo -e "\n>> $$f"; ./testParser $$f; done

clean:
	rm -f $(TARGETS)

it: all
