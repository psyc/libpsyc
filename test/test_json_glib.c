#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/socket.h>

#include <glib-object.h>
#include <json-glib/json-glib.h>

#include "test.c"

// cmd line args
char *filename, *port = "4442";
uint8_t verbose, stats;
uint8_t no_render, quiet, progress;
size_t count = 1, recv_buf_size;

int exit_code;

JsonParser *parser;
JsonGenerator *generator;

void
test_init (int i)
{
    g_type_init();
    parser = json_parser_new();
    generator = json_generator_new();
}

int
test_input (int i, char *recvbuf, size_t nbytes)
{
    JsonNode *root;
    GError *error = NULL;
    char *str;
    size_t len;
    int r, ret;

    ret = json_parser_load_from_data(parser, recvbuf, nbytes, &error);

    if (!ret) {
	printf("Parse error\n");
	exit(1);
    }

    root = json_parser_get_root(parser);

    if (!no_render) {
	json_generator_set_root(generator, root);
	str = json_generator_to_data(generator, &len);;
	if (!quiet) {
	    if (filename) {
		r = write(1, str, len);
		r = write(1, "\n", 1);
	    } else {
		send(i, str, len, 0);
		send(i, "\n", 1, 0);
	    }
	}
    }

    if (verbose)
	printf("# Done parsing.\n");
    else if (progress)
	r = write(1, ".", 1);

    return ret;
}

int main (int argc, char **argv) {
    int c;

    while ((c = getopt (argc, argv, "f:p:b:c:nqsvPh")) != -1) {
	switch (c) {
	CASE_f CASE_p CASE_b
	CASE_c CASE_n CASE_q
	CASE_s CASE_v CASE_P
	case 'h':
	    printf(HELP_FILE("test_json_glib", "mnqSsvP")
		   HELP_PORT("test_json_glib", "nqsvP")
		   HELP_f HELP_p HELP_b HELP_c
		   HELP_m HELP_n HELP_q HELP_S
		   HELP_s HELP_v HELP_P HELP_h,
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
