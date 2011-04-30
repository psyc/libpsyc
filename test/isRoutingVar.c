#include <psyc.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include "../include/psyc/lib.h"

int main() {
#if 0
	const char* vars[] =
	{
		"_source",
		"_source_relay",
		"_source_foo",
		"_sourcherry",
		"_foo",
		"bar",
		"_",
	};

	int i;
	for (i = 0; i < sizeof(vars) / sizeof(*vars); i++)
	{
		printf(">> %s: %d %d\n", vars[i], sizeof(vars[i]), sizeof(*vars[i]));
		printf("%s: %d\n", vars[i], psyc_isRoutingVar(vars[i], strlen(vars[i])));
	}
#else
	unless (psyc_isRoutingVar(PSYC_C2ARG("_source"))) return 1;
	unless (psyc_isRoutingVar(PSYC_C2ARG("_source_relay"))) return 2;
	if (psyc_isRoutingVar(PSYC_C2ARG("_source_foo"))) return 3;
	if (psyc_isRoutingVar(PSYC_C2ARG("_sourcherry"))) return 4;
	if (psyc_isRoutingVar(PSYC_C2ARG("_sour"))) return 5;
	if (psyc_isRoutingVar(PSYC_C2ARG("_foo"))) return 6;
	if (psyc_isRoutingVar(PSYC_C2ARG("bar"))) return 7;
	if (psyc_isRoutingVar(PSYC_C2ARG("_"))) return 8;

	puts("psyc_isRoutingVar passed all tests.");
#endif
	return 0; // passed all tests
}
