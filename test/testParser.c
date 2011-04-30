#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#include <psyc/lib.h>
#include <psyc/parser.h>

int main(int argc, char **argv)
{
	int idx, ret, header_only = argc > 2, verbose = argc > 3;
	char buffer[2048], oper;
	psycString name, value, elem;
	psycParseState state;
	psycParseListState listState;

	int file = open(argv[1],O_RDONLY);
	if (file < 0)
		return -1;
	idx = read(file,(void*)buffer,sizeof(buffer));

	if (verbose) {
		write(1, ">> INPUT\n", 9);
		write(1, buffer, idx);
		write(1, ">> PARSE\n", 9);
	}
	if (header_only)
		psyc_initParseState2(&state, PSYC_PARSE_HEADER_ONLY);
	else
		psyc_initParseState(&state);
	psyc_nextParseBuffer(&state, psyc_newString(buffer, idx));

	// try parsing that now
//	while ((ret = psyc_parse(&state, &oper, &name, &value)))
//	{
	do {
		oper = 0;
		name.length = 0;
		value.length = 0;

		ret = psyc_parse(&state, &oper, &name, &value);
		if (verbose)
			printf(">> ret = %d\n", ret);
		switch (ret)
		{
			case PSYC_PARSE_ROUTING:
			case PSYC_PARSE_ENTITY:
				if (verbose)
					write(1, &oper, 1);
			case PSYC_PARSE_BODY:
				// printf("the string is '%.*s'\n", name);
				if (verbose) {
					write(1, name.ptr, name.length);
					write(1, " = ", 3);
					write(1, value.ptr, value.length);
					write(1, "\n", 1);
				}
				if (memcmp(name.ptr, "_list", 5) == 0)
				{
					if (verbose)
						write(1, ">>> LIST START\n", 15);
					psyc_initParseListState(&listState);
					psyc_nextParseListBuffer(&listState, value);
					while ((ret = psyc_parseList(&listState, &name, &value, &elem)))
					{
						switch (ret)
						{
							case PSYC_PARSE_LIST_END:
							case PSYC_PARSE_LIST_ELEM:
								if (verbose) {
									write(1, "|", 1);
									write(1, elem.ptr, elem.length);
									write(1, "\n", 1);
								}
								break;
							default:
								printf("Error while parsing list: %i\n", ret);
								return 1;
						}

						if (ret == PSYC_PARSE_LIST_END)
						{
							if (verbose)
								write(1, ">>> LIST END\n", 13);
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
	} while (ret);

	return 0;
}
