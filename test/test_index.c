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

#include <psyc.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <lib.h>

uint8_t verbose;

int
test_index (const char *buf, size_t buflen)
{
    char *res = malloc(buflen * 2);
    size_t reslen = 0;

    int ret, reti, len = 0;
    PsycString idx;
    PsycParseIndexState state;
    psyc_parse_index_state_init(&state);
    psyc_parse_index_buffer_set(&state, buf, buflen);

    if (verbose)
	printf(">> %.*s\n", (int)buflen, buf);

    do {
	ret = reti = psyc_parse_index(&state, &idx);
	len = 0;

	switch (ret) {
	case PSYC_PARSE_INDEX_LIST_LAST:
	    ret = 0;
	case PSYC_PARSE_INDEX_LIST:
	    len = sprintf(res + reslen, "#%ld", idx.length);
	    break;
	case PSYC_PARSE_INDEX_STRUCT_LAST:
	    ret = 0;
	case PSYC_PARSE_INDEX_STRUCT:
	    len = sprintf(res + reslen, ".%.*s", PSYC_S2ARGP(idx));
	    break;
	case PSYC_PARSE_INDEX_DICT:
	    if (state.elemlen_found)
		len = sprintf(res + reslen, "{%ld %.*s}",
			      idx.length, PSYC_S2ARGP(idx));
	    else
		len = sprintf(res + reslen, "{%.*s}", PSYC_S2ARGP(idx));
	    break;
	case PSYC_PARSE_INDEX_END:
	    ret = 0;
	    break;
	default:
	    ret = -1;
	}
	if (verbose)
	    printf("%2d: %.*s\n", reti, len, res + reslen);
	reslen += len;
    } while (ret > 0);

    if (reslen != buflen || memcmp(res, buf, buflen) != 0) {
	printf("ERROR: got\n\[%.*s] (%ld) instead of\n[%.*s] (%ld)\n",
	       (int)reslen, res, reslen, (int)buflen, buf, buflen);
	ret = 1;
    } else
	ret = 0;

    free(res);
    return ret;
}

int
main (int argc, char **argv)
{
    verbose = argc > 1;

    if (test_index(PSYC_C2ARG("#1{foo}._bar")) != 0)
	return 1;

    if (test_index(PSYC_C2ARG("{3 foo}._bar#0")) != 0)
	return 2;

    if (test_index(PSYC_C2ARG("{foo}#2._bar")) != 0)
	return 3;

    if (test_index(PSYC_C2ARG("._bar#1{3 foo}")) != 0)
	return 4;

    printf("test_index passed all tests.\n");
    return 0; // passed all tests
}
