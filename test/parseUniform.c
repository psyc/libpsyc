#include <psyc/uniform.h>
#include <stdlib.h>
#include <stdio.h>
#include <lib.h>

void
testUniform(char *str, int ret) {
	psycUniform *uni = malloc(sizeof(psycUniform));
	memset(uni, 0, sizeof(psycUniform));
	printf("%s\n", str);
	int r = psyc_parseUniform2(uni, str, strlen(str));

	PP(("[%.*s] : [%.*s] [%.*s] : [%.*s] [%.*s] / [%.*s] # [%.*s]\n[%.*s] [%.*s]\n[%.*s]\n\n",
	    (int)PSYC_S2ARG2((*uni)[PSYC_UNIFORM_SCHEME]),
	    (int)PSYC_S2ARG2((*uni)[PSYC_UNIFORM_SLASHES]),
	    (int)PSYC_S2ARG2((*uni)[PSYC_UNIFORM_HOST]),
	    (int)PSYC_S2ARG2((*uni)[PSYC_UNIFORM_PORT]),
	    (int)PSYC_S2ARG2((*uni)[PSYC_UNIFORM_TRANSPORT]),
	    (int)PSYC_S2ARG2((*uni)[PSYC_UNIFORM_RESOURCE]),
	    (int)PSYC_S2ARG2((*uni)[PSYC_UNIFORM_CHANNEL]),
	    (int)PSYC_S2ARG2((*uni)[PSYC_UNIFORM_ROOT]),
	    (int)PSYC_S2ARG2((*uni)[PSYC_UNIFORM_NICK]),
	    (int)PSYC_S2ARG2((*uni)[PSYC_UNIFORM_BODY])));

	free(uni);
	if (r != ret) {
		fprintf(stderr, "ERROR: psyc_parseUniform returned %d instead of %d\n", r, ret);
		exit(1);
	}
}

int main() {
	testUniform("psyc://foo.tld:4404d/@bar#baz", PSYC_SCHEME_PSYC);
	testUniform("psyc://foo:4405/~bar", PSYC_SCHEME_PSYC);
	testUniform("psyc://foo:1234", PSYC_SCHEME_PSYC);
	testUniform("psyc://foo:1234d", PSYC_SCHEME_PSYC);
	testUniform("psyc://foo/", PSYC_SCHEME_PSYC);
	testUniform("psyc://foo", PSYC_SCHEME_PSYC);
	testUniform("psyc://1234567890abcdef:g/~foo", PSYC_SCHEME_PSYC);

	testUniform("xmpp:user@host", PSYC_PARSE_UNIFORM_INVALID_SCHEME);
	testUniform("psyc:host", PSYC_PARSE_UNIFORM_INVALID_SLASHES);
	testUniform("psyc://", PSYC_PARSE_UNIFORM_INVALID_HOST);
	testUniform("psyc://:123/", PSYC_PARSE_UNIFORM_INVALID_HOST);
	testUniform("psyc://host:/~foo", PSYC_PARSE_UNIFORM_INVALID_PORT);
	testUniform("psyc://host:d/~foo", PSYC_PARSE_UNIFORM_INVALID_PORT);
	testUniform("psyc://1234567890abcdef:1g/~foo", PSYC_PARSE_UNIFORM_INVALID_TRANSPORT);

	printf("SUCCESS: psyc_parseUniform passed all tests.\n");
	return 0;
}
