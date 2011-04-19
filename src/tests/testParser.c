#include <psyc/parser.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
	int index, ret;
	uint8_t buffer[2048], modifier;
	PSYC_Array name, value, elem;
	PSYC_State state;
	PSYC_ListState listState;

	int file = open(argv[1],O_RDONLY);
	if(file < 0)
		return -1;
	index = read(file,(void*)buffer,sizeof(buffer));

	write(1, ">> INPUT\n", 9);
	write(1, buffer, index);
	write(1, ">> PARSE\n", 9);

	PSYC_initState(&state);
	PSYC_nextBuffer(&state, PSYC_createArray(buffer, index));

	// try parsing that now
	while (ret = PSYC_parse(&state, &modifier, &name, &value))
	{
		switch (ret)
		{
			case PSYC_ROUTING:
			case PSYC_ENTITY:
				write(1, &modifier, 1);
			case PSYC_BODY:
				write(1, name.ptr, name.length);
				write(1, " = ", 3);
				write(1, value.ptr, value.length);
				write(1, "\n", 1);
				if (memcmp(name.ptr, "_list", 5) == 0)
				{
					write(1, ">>> LIST START\n", 15);
					PSYC_initListState(&listState);
					PSYC_nextListBuffer(&listState, value);
					while (ret = PSYC_parseList(&listState, &name, &value, &elem))
					{
						switch (ret)
						{
							case PSYC_LIST_END:
							case PSYC_LIST_ELEM:
								write(1, "|", 1);
								write(1, elem.ptr, elem.length);
								write(1, "\n", 1);
								break;
							default:
								printf("Error while parsing list: %i\n", ret);
								return 1;
						}

						if (ret == PSYC_LIST_END)
						{
							write(1, ">>> LIST END\n", 13);
							break;
						}
					}
				}
				break;
			case PSYC_COMPLETE:
				printf("Done parsing.\n");
				continue;
			case PSYC_INSUFFICIENT:
				printf("Insufficient data.\n");
				return 0;
			default:
				printf("Error while parsing: %i\n", ret);
				return 1;
		}
	}
	return 0;
}
