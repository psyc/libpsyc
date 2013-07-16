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

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#include <psyc.h>
#include <psyc/parse.h>

int
main (int argc, char **argv)
{
    uint8_t routing_only = argc > 2 && memchr(argv[2], (int)'r', strlen(argv[2]));
    uint8_t verbose = argc > 2 && memchr(argv[2], (int)'v', strlen(argv[2]));
    int idx, ret;
    char buffer[2048], oper;
    PsycString name, value, type;
    PsycParseState state;
    PsycParseListState listState;

    if (argc < 2)
	return -1;
    int file = open(argv[1],O_RDONLY);
    if (file < 0)
	return -1;
    idx = read(file,(void*)buffer,sizeof(buffer));

    if (verbose) {
	printf(">> INPUT\n");
	printf("%.*s\n", (int)idx, buffer);
	printf(">> PARSE\n");
    }

    psyc_parse_state_init(&state, routing_only ?
			  PSYC_PARSE_ROUTING_ONLY : PSYC_PARSE_ALL);
    psyc_parse_buffer_set(&state, buffer, idx);

    // try parsing that now
    do {
	oper = 0;
	name.length = 0;
	value.length = 0;

	ret = psyc_parse(&state, &oper, &name, &value);
	if (verbose)
	    printf(">> ret = %d\n", ret);

	switch (ret) {
	case PSYC_PARSE_ROUTING:
	case PSYC_PARSE_ENTITY:
	    if (verbose)
		printf("%c", oper);
	case PSYC_PARSE_BODY:
	    // printf("the string is '%.*s'\n", name);
	    if (verbose)
		printf("%.*s = %.*s\n",
		       (int)name.length, name.data,
		       (int)value.length, value.data);

	    if (psyc_var_is_list(PSYC_S2ARG(name))) {
		if (verbose)
		    printf(">> LIST START\n");

		psyc_parse_list_state_init(&listState);
		psyc_parse_list_buffer_set(&listState, PSYC_S2ARG(value));

		while ((ret = psyc_parse_list(&listState, &type, &value))) {
		    switch (ret) {
		    case PSYC_PARSE_LIST_END:
		    case PSYC_PARSE_LIST_ELEM:
			if (verbose)
			    printf("|%.*s %.*s\n", (int)type.length, type.data,
				   (int)value.length, value.data);
			break;
		    default:
			printf("Error while parsing list: %i\n", ret);
			return 1;
		    }

		    if (ret == PSYC_PARSE_LIST_END) {
			if (verbose)
			    printf(">> LIST END\n");
			break;
		    }
		}
	    }
	    break;
	case PSYC_PARSE_COMPLETE:
	    // printf("Done parsing.\n");
	    ret = 0;
	    continue;
	case PSYC_PARSE_INSUFFICIENT:
	    printf("Insufficient data.\n");
	    return 1;
	default:
	    printf("Error while parsing: %i\n", ret);
	    return 1;
	}
    }
    while (ret);

    return 0;
}
