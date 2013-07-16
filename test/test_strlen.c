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
