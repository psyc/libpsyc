#include <psyc.h>
#include <psyc/parser.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
	int indx, ret;
	char buffer[2048], oper;
	psycString name, value, elem;
	psycParseState state;
	psycParseListState listState;

	int file = open(argv[1],O_RDONLY);
	if(file < 0)
		return -1;
	indx = read(file,(void*)buffer,sizeof(buffer));

	write(1, ">> INPUT\n", 9);
	write(1, buffer, indx);
	write(1, ">> PARSE\n", 9);

	psyc_initParseState(&state);
	psyc_nextParseBuffer(&state, psyc_newString(buffer, indx));

	// try parsing that now
	while ((ret = psyc_parse(&state, &oper, &name, &value)))
	{
		switch (ret)
		{
			case PSYC_PARSE_ROUTING:
			case PSYC_PARSE_ENTITY:
				write(1, &oper, 1);
			case PSYC_PARSE_BODY:
				// printf("the string is '%.*s'\n", name);
				write(1, name.ptr, name.length);
				write(1, " = ", 3);
				write(1, value.ptr, value.length);
				write(1, "\n", 1);
				if (memcmp(name.ptr, "_list", 5) == 0)
				{
					write(1, ">>> LIST START\n", 15);
					psyc_initParseListState(&listState);
					psyc_nextParseListBuffer(&listState, value);
					while ((ret = psyc_parseList(&listState, &name, &value, &elem)))
					{
						switch (ret)
						{
							case PSYC_PARSE_LIST_END:
							case PSYC_PARSE_LIST_ELEM:
								write(1, "|", 1);
								write(1, elem.ptr, elem.length);
								write(1, "\n", 1);
								break;
							default:
								printf("Error while parsing list: %i\n", ret);
								return 1;
						}

						if (ret == PSYC_PARSE_LIST_END)
						{
							write(1, ">>> LIST END\n", 13);
							break;
						}
					}
				}
				break;
			case PSYC_PARSE_COMPLETE:
				printf("Done parsing.\n");
				continue;
			case PSYC_PARSE_INSUFFICIENT:
				printf("Insufficient data.\n");
				return 0;
			default:
				printf("Error while parsing: %i\n", ret);
				return 1;
		}
	}
	return 0;
}
