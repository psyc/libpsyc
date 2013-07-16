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

#include <stdio.h>
#include <stdlib.h>

#include <psyc/parse.h>

int
parse_table (char *buf, size_t buflen)
{
    printf(">> %.*s\n", (int)buflen, buf);

    int ret;
    PsycString elem;
    PsycParseTableState state;
    psyc_parse_table_state_init(&state);
    psyc_parse_table_buffer_set(&state, buf, buflen);

    do {
	ret = psyc_parse_table(&state, &elem);
	switch (ret) {
	case PSYC_PARSE_TABLE_WIDTH:
	    printf("width: %ld\n", elem.length);
	    break;
#ifdef PSYC_PARSE_TABLE_HEAD
	case PSYC_PARSE_TABLE_NAME_KEY:
	    printf("name key: %.*s\n", (int)PSYC_S2ARG2(elem));
	    break;
	case PSYC_PARSE_TABLE_NAME_VALUE:
	    printf("name val: %.*s\n", (int)PSYC_S2ARG2(elem));
	    break;
#endif
	case PSYC_PARSE_TABLE_KEY_END:
	    ret = 0;
	case PSYC_PARSE_TABLE_KEY:
	    printf("key: %.*s\n", (int)PSYC_S2ARG2(elem));
	    break;
	case PSYC_PARSE_TABLE_VALUE_END:
	    ret = 0;
	case PSYC_PARSE_TABLE_VALUE:
	    printf("val: %.*s\n", (int)PSYC_S2ARG2(elem));
	    break;
	default:
	    printf("err: %d\n", ret);
	}
    } while (ret > 0);
    return ret == 0;
}

int
main (int argc, char **argv)
{

#ifdef PSYC_PARSE_TABLE_HEAD
    if (!parse_table(PSYC_C2ARG("*2|_key|_val1|_val2 |_foo|bar|baz|_aaa|bbb|ccc")))
	return 1;
#endif

    if (!parse_table(PSYC_C2ARG("*2 |_foo|bar|baz|_aaa|bbb|ccc")))
	return 2;

#ifdef PSYC_PARSE_TABLE_HEAD
    if (!parse_table(PSYC_C2ARG("*1|_key|_val1 |_foo|bar|_baz|aaa|_bbb|ccc")))
	return 1;
#endif

    if (!parse_table(PSYC_C2ARG("*1 |_foo|bar|_baz|aaa|_bbb|ccc")))
	return 3;

#ifdef PSYC_PARSE_TABLE_HEAD
    if (!parse_table(PSYC_C2ARG("*0|_key |foo|bar|baz|aaa|bbb|ccc")))
	return 4;
#endif

    if (!parse_table(PSYC_C2ARG("*0 |foo|bar|baz|aaa|bbb|ccc")))
	return 4;

    if (!parse_table(PSYC_C2ARG("|foo|bar|baz|aaa|bbb|ccc")))
	return 5;

    return 0;
}
