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


	PSYC_State state;
	PSYC_initState(&state);

	unsigned int cursor=0,tmp=0;
	unsigned long expectedBytes=0;
	uint8_t modifier;
	int ret;
	PSYC_Array name, value;

	PSYC_nextBuffer(&state, PSYC_CreateArray(buffer, index));

	// try parsing that now	
	while(ret=PSYC_parse(&state, &name, &value, &modifier, &expectedBytes))
	{
		switch (ret)
		{
			case PSYC_ROUTING:
			case PSYC_ENTITY:
			case PSYC_METHOD:
				write(0,&modifier,1);
				write(0,name.ptr, name.length);
				write(0," = ", 3);
				write(0,value.ptr, value.length);
				write(0,"\n", 1);
				break;
			case PSYC_COMPLETE:
				write(0, "Done parsing.\n", 15);
				continue;
			default:
				printf("Error while parsing: %i\n", ret);
				return;
		}

	}
	return 0;

}
