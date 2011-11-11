#define _GNU_SOURCE

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/socket.h>

#define NOREAD
#include "test.c"

// cmd line args
char *filename, *port = "4443";
uint8_t verbose, stats;
uint8_t no_render, quiet, progress;
size_t count = 1, recv_buf_size;

int exit_code;

void
test_init (int i)
{

}

int
test_input (int i, char *recvbuf, size_t nbytes)
{
    size_t len = strnlen(recvbuf, nbytes);

    if (!len) {
	printf("Empty string\n");
	return -1;
    }

    return 0;
}

int
main (int argc, char **argv)
{
    int c;

    while ((c = getopt (argc, argv, "f:p:b:c:sh")) != -1) {
	switch (c) {
	CASE_f CASE_p CASE_b CASE_c CASE_s
	case 'h':
	    printf(HELP_FILE("test_strlen", "s")
		   HELP_PORT("test_strlen", "s")
		   HELP_f HELP_p HELP_b HELP_c
		   HELP_s HELP_h,
		   port, RECV_BUF_SIZE);
	    exit(0);
	case '?': exit(-1);
	default:  abort();
	}
    }

    if (filename)
	test_file(filename, count, recv_buf_size);
    else
	test_server(port, count, recv_buf_size);

    return exit_code;
}
