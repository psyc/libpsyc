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

#include <psyc/uniform.h>
#include <stdlib.h>
#include <stdio.h>
#include <lib.h>

void
testUniform (char *str, int ret)
{
    PsycUniform *uni = malloc(sizeof(PsycUniform));
    memset(uni, 0, sizeof(PsycUniform));
    printf("%s\n", str);
    int r = psyc_uniform_parse(uni, str, strlen(str));

    PP(("[%.*s] : [%.*s] [%.*s] : [%.*s] [%.*s] / "
	"[%.*s] # [%.*s]\n"
	"[%.*s]\n"
	"[%.*s] [%.*s]\n"
	"[%.*s]\n"
	"[%.*s]\n\n",
	(int)PSYC_S2ARG2(uni->scheme),
	(int)PSYC_S2ARG2(uni->slashes),
	(int)PSYC_S2ARG2(uni->host),
	(int)PSYC_S2ARG2(uni->port),
	(int)PSYC_S2ARG2(uni->transport),
	(int)PSYC_S2ARG2(uni->resource),
	(int)PSYC_S2ARG2(uni->channel),
	(int)PSYC_S2ARG2(uni->entity),
	(int)PSYC_S2ARG2(uni->root),
	(int)PSYC_S2ARG2(uni->nick),
	(int)PSYC_S2ARG2(uni->path),
	(int)PSYC_S2ARG2(uni->body)));

    free(uni);
    if (r != ret) {
	fprintf(stderr, "ERROR: psyc_uniform_parse returned %d instead of %d\n",
		r, ret);
	exit(1);
    }
}

int
main ()
{
    testUniform("psyc://foo.tld:4404d/@bar#baz", PSYC_SCHEME_PSYC);
    testUniform("psyc://foo.tld:4404d/#baz", PSYC_SCHEME_PSYC);
    testUniform("psyc://foo:4405/~bar", PSYC_SCHEME_PSYC);
    testUniform("psyc://foo:1234", PSYC_SCHEME_PSYC);
    testUniform("psyc://foo:1234d", PSYC_SCHEME_PSYC);
    testUniform("psyc://foo:-1234", PSYC_SCHEME_PSYC);
    testUniform("psyc://foo:-1234d", PSYC_SCHEME_PSYC);
    testUniform("psyc://foo/", PSYC_SCHEME_PSYC);
    testUniform("psyc://foo", PSYC_SCHEME_PSYC);
    testUniform("psyc://1234567890abcdef:g/~foo", PSYC_SCHEME_PSYC);

    testUniform("xmpp:user@host", PSYC_PARSE_UNIFORM_INVALID_SCHEME);
    testUniform("psyc:host", PSYC_PARSE_UNIFORM_INVALID_SLASHES);
    testUniform("psyc://", PSYC_PARSE_UNIFORM_INVALID_HOST);
    testUniform("psyc://:123/", PSYC_PARSE_UNIFORM_INVALID_HOST);
    testUniform("psyc://host:/~foo", PSYC_PARSE_UNIFORM_INVALID_PORT);
    testUniform("psyc://host:d/~foo", PSYC_PARSE_UNIFORM_INVALID_PORT);
    testUniform("psyc://1234567890abcdef:1g/~foo",
		PSYC_PARSE_UNIFORM_INVALID_TRANSPORT);

    printf("SUCCESS: psyc_uniform_parse passed all tests.\n");
    return 0;
}
