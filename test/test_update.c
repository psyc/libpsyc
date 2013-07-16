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
test_update (const char *buf, size_t buflen,
	     const char *r_idx, size_t r_idxlen, char r_oper,
	     const char *r_typ, size_t r_typlen,
	     const char *r_val, size_t r_vallen)
{
    char *idx = malloc(r_idxlen * 2);
    char *typ = NULL, *val = NULL;
    size_t idxlen = 0, typlen = 0, vallen = 0;

    int ret, reti, len = 0;
    char oper;
    PsycString value;
    PsycParseUpdateState state;
    psyc_parse_update_state_init(&state);
    psyc_parse_update_buffer_set(&state, buf, buflen);

    if (verbose)
	printf(">> %.*s\n", (int)buflen, buf);

    do {
	ret = reti = psyc_parse_update(&state, &oper, &value);
	len = 0;

	switch (ret) {
	case PSYC_PARSE_INDEX_LIST:
	    len = sprintf(idx + idxlen, "#%ld", value.length);
	    break;
	case PSYC_PARSE_INDEX_STRUCT:
	    len = sprintf(idx + idxlen, ".%.*s", PSYC_S2ARGP(value));
	    break;
	case PSYC_PARSE_INDEX_DICT:
	    if (state.elemlen_found)
		len = sprintf(idx + idxlen, "{%ld %.*s}",
			      value.length, PSYC_S2ARGP(value));
	    else
		len = sprintf(idx + idxlen, "{%.*s}", PSYC_S2ARGP(value));
	    break;
	case PSYC_PARSE_UPDATE_TYPE_END:
	    ret = 0;
	case PSYC_PARSE_UPDATE_TYPE:
	    typ = value.data;
	    typlen = value.length;
	    if (verbose)
		printf("%c[%.*s]\n", oper, PSYC_S2ARGP(value));
	    break;
	case PSYC_PARSE_UPDATE_VALUE:
	    val = value.data;
	    vallen = value.length;
	    ret = 0;
	    if (verbose)
		printf("[%.*s]\n", PSYC_S2ARGP(value));
	    break;
	case PSYC_PARSE_UPDATE_END:
	    ret = 0;
	    break;
	default:
	    ret = -1;
	}
	if (verbose && len)
		printf("%2d: %.*s\n", reti, len, idx + idxlen);
	idxlen += len;
    } while (ret > 0);

    if (ret == 0) {
	if (idxlen != r_idxlen || oper != r_oper
	    || typlen != r_typlen || vallen != r_vallen
	    || memcmp(r_idx, idx, idxlen) != 0
	    || memcmp(r_typ, typ, typlen) != 0
	    || memcmp(r_val, val, vallen) != 0) {
	    printf("ERROR: got\n\[%.*s] %c[%.*s]:[%ld] [%.*s] instead of\n[%.*s]\n",
		   (int)idxlen, idx, oper, (int)typlen, typ, vallen, (int)vallen, val,
		   (int)buflen, buf);
	    ret = 1;
	} else
	    ret = 0;
    }

    free(idx);
    return ret;
}

int
main (int argc, char **argv)
{
    verbose = argc > 1;

    if (test_update(PSYC_C2ARG("#1{foo}._bar =_foo:3 bar"),
		    PSYC_C2ARG("#1{foo}._bar"), '=',
		    PSYC_C2ARG("_foo"),
		    PSYC_C2ARG("bar")) != 0)
	return 1;

    if (test_update(PSYC_C2ARG("{3 foo}._bar#0 +:3 baz"),
		    PSYC_C2ARG("{3 foo}._bar#0"), '+',
		    PSYC_C2ARG(""),
		    PSYC_C2ARG("baz")) != 0)
	return 2;

    if (test_update(PSYC_C2ARG("{foo}#2._bar - 1337"),
		    PSYC_C2ARG("{foo}#2._bar"), '-',
		    PSYC_C2ARG(""),
		    PSYC_C2ARG("1337")) != 0)
	return 3;

    if (test_update(PSYC_C2ARG("._bar#1{3 foo} ="),
		    PSYC_C2ARG("._bar#1{3 foo}"), '=',
		    PSYC_C2ARG(""),
		    PSYC_C2ARG("")) != 0)
	return 4;

    if (test_update(PSYC_C2ARG("#1{3 foo}._bar =_list"),
		    PSYC_C2ARG("#1{3 foo}._bar"), '=',
		    PSYC_C2ARG("_list"),
		    PSYC_C2ARG("")) != 0)
	return 5;

    if (test_update(PSYC_C2ARG("#1{3 foo}._bar =_list "),
		    PSYC_C2ARG("#1{3 foo}._bar"), '=',
		    PSYC_C2ARG("_list"),
		    PSYC_C2ARG("")) != 0)
	return 6;

    if (test_update(PSYC_C2ARG("#1{3 foo}._bar =_list"),
		    PSYC_C2ARG("#1{3 foo}._bar"), '=',
		    PSYC_C2ARG("_list"),
		    PSYC_C2ARG("")) != 0)
	return 7;

    printf("test_update passed all tests.\n");
    return 0; // passed all tests
}
