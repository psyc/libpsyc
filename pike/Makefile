CC=gcc
CFLAGS := -O9 -shared -Wall -fPIC -I/usr/include/pike $(CFLAGS) $(shell $(PIKE) -x cflags) 
LD=$(CC) -shared -lpsyc

all: module.so

module.so: module.o
	$(LD) -o module.so module.o $(LDFLAGS)

module.o: module.c
	$(CC) $(CFLAGS) -c module.c 

module.c: psyc.cmod
	$(PIKE) -x precompile $< > $@
