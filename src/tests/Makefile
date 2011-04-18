CFLAGS=-I../../include -DDEBUG -g -O0
LDFLAGS=-L..
LOADLIBES=-lpsyc
TARGETS=testParser testMatch

all: $(TARGETS)

# special case because not in the same directory
testParser:
	${CC} ${CFLAGS} ${LDFLAGS} parser/testParser.c ${LOADLIBES} -o testParser

test:
	for f in parser/*txt; do echo -e "\n>> $$f"; ./testParser $$f; done

clean:
	rm $(TARGETS)
