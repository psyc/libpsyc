/*
  This file is part of libpsyc.
  Copyright (C) 2011,2012 Carlo v. Loesch, Gabor X Toth, Mathias L. Baumann,
  and other contributing authors.

  libpsyc is free software: you can redistribute it and/or modify it under the
  terms of the GNU Affero General Public License as published by the Free
  Software Foundation, either version 3 of the License, or (at your option) any
  later version. As a special exception, libpsyc is distributed with additional
  permissions to link libpsyc libraries with non-AGPL works.

  libpsyc is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
  details.

  You should have received a copy of the GNU Affero General Public License and
  the linking exception along with libpsyc in a COPYING file.
*/

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
size_t count = 1, recv_buf_size;

PsycParseState parser;

void
test_init (int i)
{
    psyc_parse_state_init(&parser, routing_only
			  ? PSYC_PARSE_ROUTING_ONLY : PSYC_PARSE_ALL);
}

int
test_input (int i, char *recvbuf, size_t nbytes)
{
    char oper;
    PsycString name, value;
    int ret;

    psyc_parse_buffer_set(&parser, recvbuf, nbytes);

    for (;;) {
	ret = psyc_parse(&parser, &oper, &name, &value);
	if (ret == PSYC_PARSE_COMPLETE || ret < 0)
	    return -1;
    }
}

int
main (int argc, char **argv)
{
    int c;
    while ((c = getopt (argc, argv, "f:p:b:c:rsh")) != -1) {
	switch (c) {
	CASE_f CASE_p CASE_b CASE_c CASE_r CASE_s
	case 'h':
	    printf(HELP_FILE("test_psyc_speed", "rs")
		   HELP_PORT("test_psyc_speed", "rs")
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
