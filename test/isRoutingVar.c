#include <psyc.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
	char* vars[] =
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
		printf("%s: %d\n", vars[i], PSYC_isRoutingVar(vars[i], strlen(vars[i])));
	}

	return 0;
}
