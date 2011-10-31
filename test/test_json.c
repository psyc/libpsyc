#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/socket.h>

#include <json/json.h>
#include <json/json_object.h>
#include <json/json_tokener.h>

#include "test.c"

// cmd line args
char *filename, *port = "4441";
uint8_t verbose, stats;
uint8_t multiple, single, no_render, quiet, progress;
size_t count = 1, recv_buf_size;

int exit_code;

json_object *obj;
json_tokener *tok;
enum json_tokener_error error;

void test_init (int i) {
	tok = json_tokener_new();
}

int test_input (int i, char *recvbuf, size_t nbytes) {
	size_t cursor = 0;
	int r, ret = 0;

	json_tokener_reset(tok);

	do {
		obj = json_tokener_parse_ex(tok, recvbuf + cursor, nbytes - cursor);
		cursor += tok->char_offset;

		if (verbose)
			printf("#%d\n", tok->err);

		switch (tok->err) {
			case json_tokener_continue:
				return 0;

			case json_tokener_success:
				if (!no_render) {
					const char *str = json_object_to_json_string(obj);
					if (!quiet) {
						size_t len = strlen(str);
						if (filename) {
							r = write(1, str, len);
							r = write(1, "\n", 1);
						} else {
							send(i, str, len, 0);
							send(i, "\n", 1, 0);
						}
					}
				}

				json_object_put(obj);

				if (verbose)
					printf("# Done parsing.\n");
				else if (progress)
					r = write(1, ".", 1);
				if ((filename && !multiple) || (!filename && single))
					return -1;
				break;

			default:
				printf("parse error\n");
				exit_code = tok->err;
				return -1;
		}
	} while (cursor < nbytes);

	return ret;
}

int main (int argc, char **argv) {
	int c;

	while ((c = getopt (argc, argv, "f:p:b:c:mnqsvPSh")) != -1) {
		switch (c) {
			CASE_f CASE_p CASE_b CASE_c
			CASE_m CASE_n CASE_q CASE_s
			CASE_v CASE_S CASE_P
			case 'h':
				printf(
					HELP_FILE("testJson", "mnqSsvP")
					HELP_PORT("testJson", "nqsvP")
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
