#include <stdio.h>

#include <psyc/lib.h>
#include <psyc/text.h>

#define BUFSIZE 512

uint8_t verbose;

psycTextValueRC getValueFooBar (const char *name, size_t len, psycString *value)
{
	value->ptr = "Foo Bar";
	value->length = 7;
	return PSYC_TEXT_VALUE_FOUND;
}

psycTextValueRC getValueEmpty (const char *name, size_t len, psycString *value)
{
	value->ptr = "";
	value->length = 0;
	return PSYC_TEXT_VALUE_FOUND;
}

psycTextValueRC getValueNotFound (const char *name, size_t len, psycString *value)
{
	return PSYC_TEXT_VALUE_NOT_FOUND;
}

int testText (char *template, size_t tmplen, char *buffer, size_t buflen, psycString *result, psycTextCB getValue)
{
	psycTextState state;
	size_t length = 0;
	psycTextRC ret;

	psyc_initTextState(&state, template, tmplen, buffer, buflen);
	do
	{
		ret = psyc_text(&state, getValue);
		length += state.written;
		switch (ret)
		{
			case PSYC_TEXT_INCOMPLETE:
				if (verbose)
					printf("# %.*s...\n", (int)length, buffer);
				psyc_setTextBuffer2(&state, buffer + length, BUFSIZE - length);
				break;
			case PSYC_TEXT_COMPLETE:
				if (verbose)
					printf("%.*s\n", (int)length, buffer);
				result->length = length;
				result->ptr = buffer;
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

int main(int argc, char **argv)
{
	verbose = argc > 1;
	char buffer[BUFSIZE];
	psycString result;

	char *str = "Hello [_foo] & [_bar]!";
	size_t len = strlen(str);
	int i;

	testText(str, len, buffer, BUFSIZE, &result, &getValueFooBar);
	if (memcmp(result.ptr, PSYC_C2ARG("Hello Foo Bar & Foo Bar!")))
		return 1;

	testText(str, len, buffer, BUFSIZE, &result, &getValueEmpty);
	if (memcmp(result.ptr, PSYC_C2ARG("Hello  & !")))
		return 2;

	if (testText(str, len, buffer, BUFSIZE, &result, &getValueNotFound) != PSYC_TEXT_NO_SUBST)
		return 3;

	for (i = 1; i < 22; i++)
	{
		testText(str, len, buffer, i, &result, &getValueFooBar);
		if (memcmp(result.ptr, PSYC_C2ARG("Hello Foo Bar & Foo Bar!")))
			return 10 + i;
	}

	puts("psyc_text passed all tests.");

	return 0;
}
