#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#include <psyc.h>
#include <psyc/parser.h>

int main (int argc, char **argv)
{
	uint8_t routing_only = argc > 2 && memchr(argv[2], (int)'r', strlen(argv[2]));
	uint8_t verbose = argc > 2 && memchr(argv[2], (int)'v', strlen(argv[2]));
	int idx, ret;
	char buffer[2048], oper;
	psycString name, value, elem;
	psycParseState state;
	psycParseListState listState;

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

	if (routing_only)
		psyc_initParseState2(&state, PSYC_PARSE_ROUTING_ONLY);
	else
		psyc_initParseState(&state);

	psyc_setParseBuffer(&state, psyc_newString(buffer, idx));

	// try parsing that now
	do
	{
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
					printf("%c", oper);
			case PSYC_PARSE_BODY:
				// printf("the string is '%.*s'\n", name);
				if (verbose)
					printf("%.*s = %.*s\n",
					       (int)name.length, name.ptr,
					       (int)value.length, value.ptr);

				if (name.length >= 5 && memcmp(name.ptr, "_list", 5) == 0)
				{
					if (verbose)
						printf(">> LIST START\n");

					psyc_initParseListState(&listState);
					psyc_setParseListBuffer(&listState, value);

					while ((ret = psyc_parseList(&listState, &name, &value, &elem)))
					{
						switch (ret)
						{
							case PSYC_PARSE_LIST_END:
							case PSYC_PARSE_LIST_ELEM:
								if (verbose)
									printf("|%.*s\n", (int)elem.length, elem.ptr);
								break;
							default:
								printf("Error while parsing list: %i\n", ret);
								return 1;
						}

						if (ret == PSYC_PARSE_LIST_END)
						{
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
