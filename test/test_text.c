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

#include <lib.h>
#include <psyc/text.h>

#define BUFSIZE 512

uint8_t verbose;

PsycTextValueRC
getValueFooBar (void *cls, const char *name, size_t len, PsycString *value)
{
    if (verbose)
	printf("> getValue: %.*s\n", (int)len, name);
    value->data = "Foo Bar";
    value->length = 7;
    return PSYC_TEXT_VALUE_FOUND;
}

PsycTextValueRC
getValueEmpty (void *cls, const char *name, size_t len, PsycString *value)
{
    if (verbose)
	printf("> getValue: %.*s\n", (int)len, name);
    value->data = "";
    value->length = 0;
    return PSYC_TEXT_VALUE_FOUND;
}

PsycTextValueRC
getValueNotFound (void *cls, const char *name, size_t len, PsycString *value)
{
    if (verbose)
	printf("> getValue: %.*s\n", (int)len, name);
    return PSYC_TEXT_VALUE_NOT_FOUND;
}

int
testText (char *template, size_t tmplen, char *buffer, size_t buflen,
	  PsycString *result, PsycTextCB getValue)
{
    PsycTextState state;
    size_t length = 0;
    PsycTextRC ret;

    psyc_text_state_init(&state, template, tmplen, buffer, buflen);
    do {
	ret = psyc_text(&state, getValue, NULL);
	length += psyc_text_bytes_written(&state);
	switch (ret) {
	case PSYC_TEXT_INCOMPLETE:
	    if (verbose)
		printf("# %.*s...\n", (int)length, buffer);
	    psyc_text_buffer_set(&state, buffer + length, BUFSIZE - length);
	    break;
	case PSYC_TEXT_COMPLETE:
	    if (verbose)
		printf("%.*s\n", (int)length, buffer);
	    result->length = length;
	    result->data = buffer;
	    return ret;
	case PSYC_TEXT_NO_SUBST:
	    if (verbose)
		printf("%.*s\n", (int)tmplen, template);
	    return ret;
	}
    }
    while (ret == PSYC_TEXT_INCOMPLETE);

    return -2; // shouldn't be reached
}

int
main (int argc, char **argv)
{
    verbose = argc > 1;
    char buffer[BUFSIZE];
    PsycString result;

    char *str = "Hello [_foo] & [_bar]!";
    size_t len = strlen(str);
    int i;

    testText(str, len, buffer, BUFSIZE, &result, &getValueFooBar);
    if (memcmp(result.data, PSYC_C2ARG("Hello Foo Bar & Foo Bar!")))
	return 1;

    testText(str, len, buffer, BUFSIZE, &result, &getValueEmpty);
    if (memcmp(result.data, PSYC_C2ARG("Hello  & !")))
	return 2;

    if (PSYC_TEXT_NO_SUBST != testText(str, len, buffer, BUFSIZE,
				       &result, &getValueNotFound))
	return 3;

    for (i = 1; i < 22; i++) {
	testText(str, len, buffer, i, &result, &getValueFooBar);
	if (memcmp(result.data, PSYC_C2ARG("Hello Foo Bar & Foo Bar!")))
	    return 10 + i;
    }

    size_t tlen = 0;
    const char *t = psyc_template(PSYC_MC_NOTICE_CONTEXT_ENTER, &tlen);
    printf("_notice_context_enter = %s, %ld\n", t, tlen);

    printf("psyc_text passed all tests.\n");

    return 0;
}
