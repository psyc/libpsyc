#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/socket.h>

#define __INLINE_PSYC_PARSE
#include "../src/parse.c"
//#include <psyc/parse.h>

#include "test.c"

// max size for routing & entity header
#define ROUTING_LINES 16
#define ENTITY_LINES 32

// cmd line args
char *filename, *port = "4440";
uint8_t verbose, stats;
uint8_t routing_only;
size_t count = 1, recv_buf_size = RECV_BUF_SIZE;

psycParseState parser;

void test_init (int i) {
	if (routing_only)
		psyc_initParseState2(&parser, PSYC_PARSE_ROUTING_ONLY);
	else
		psyc_initParseState(&parser);
}

int test_input (int i, char *recvbuf, size_t nbytes) {
	char oper;
	psycString name, value;
	int ret;

	psyc_setParseBuffer2(&parser, recvbuf, nbytes);

	for (;;) {
		ret = psyc_parse(&parser, &oper, &name, &value);
		if (ret == PSYC_PARSE_COMPLETE || ret < 0)
			return -1;
	}
}

int main (int argc, char **argv) {
	int c;
	while ((c = getopt (argc, argv, "f:p:b:c:rsh")) != -1) {
		switch (c) {
			CASE_f CASE_p CASE_b CASE_c
			CASE_r CASE_s
			case 'h':
				printf(
					HELP_FILE("testPsycSpeed", "rs")
					HELP_PORT("testPsycSpeed", "rs")
					HELP_f HELP_p HELP_b HELP_c
					HELP_r HELP_s HELP_h,
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

	return 0;
}
