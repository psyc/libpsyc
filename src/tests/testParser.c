#include <psyc/parser.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
	uint8_t buffer[2048];
	int index;

	int file = open(argv[1],O_RDONLY);
	if(file < 0)
		return -1;
	index = read(file,(void*)buffer,sizeof(buffer));

	write(1, ">> INPUT:\n", 10);
	write(1, buffer, index);
	write(1, ">> PARSE:\n", 10);

	PSYC_State state;
	PSYC_initState(&state);

	//unsigned int cursor=0,tmp=0;
	//unsigned long expectedBytes=0;
	uint8_t modifier;
	int ret;
	PSYC_Array name, value;

	PSYC_nextBuffer(&state, PSYC_createArray(buffer, index));

	// try parsing that now
	while((ret=PSYC_parse(&state, &modifier, &name, &value)))
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
