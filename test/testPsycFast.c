#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/socket.h>

//#define PSYC_PARSE_INLINE
//#include "../src/parse.c"
#include <psyc/parse.h>

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
	// reset parser state
	if (routing_only)
		psyc_initParseState2(&parser, PSYC_PARSE_ROUTING_ONLY);
	else
		psyc_initParseState(&parser);
}

int test_input (int i, char *recvbuf, size_t nbytes) {
	char oper;
	psycString name, value;
	int ret;

	parser.buffer = (psycString){nbytes, recvbuf};
	parser.cursor = 0;

	for (;;) {
		ret = psyc_parse(&parser, &oper, &name, &value);

		if (ret == PSYC_PARSE_COMPLETE || ret < 0)
			return -1;
	}
}

int main (int argc, char **argv) {
	int c;
	while ((c = getopt (argc, argv, "f:p:b:c:qrsvh")) != -1) {
		switch (c) {
			case 'f': filename = optarg; break;
			case 'p': port = optarg; check_range(c, optarg, 1, 0); break;
			case 'b': recv_buf_size = atoi(optarg); check_range(c, optarg, 1, RECV_BUF_SIZE); break;
			case 'c': count = atoi(optarg); check_range(c, optarg, 1, 0); break;
			case 'r': routing_only = 1; break;
			case 's': stats = 1; break;
			case 'v': verbose++; break;
			case 'h':
				printf(
					"testPsyc -f <filename> [-b <read_buf_size>] [-c <count>] [-mnqrSsvP]\n"
					"testPsyc [-p <port>] [-b <recv_buf_size>] [-nqrsvP]\n"
					"  -f <filename>\tInput file name\n"
					"  -p <port>\t\tListen on TCP port, default is %s\n"
					"  -b <buf_size>\tRead/receive buffer size, default is %d\n"
					"  -c <count>\t\tParse data from file <count> times\n"
					"  -r\t\t\tParse routing header only\n"
					"  -s\t\t\tShow statistics at the end\n"
					"  -h\t\t\tShow this help\n"
					"  -v\t\t\tVerbose\n",
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
