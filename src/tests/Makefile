CFLAGS=-I.. -DDEBUG
LDFLAGS=-L.. -lpsyc

works:
	cc -I../../include -g -O0 -DDEBUG -L.. parser/testParser.c -o testParser -lpsyc

it: testParser

test:
	for f in parser/*txt; do echo -e "\n>> $$f"; ./testParser $$f; done
