#include <parser.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
	const uint8_t buffer[2048];
	int index;

	int file = open(argv[1],O_RDONLY);
	if(file < 0)
		return -1;	
	index = read(file,(void*)buffer,sizeof(buffer));

	unsigned int cursor=0,tmp=0;
	while(cursor < index) 
	{
		int ret;
		const uint8_t *varname,*varvalue;
		unsigned int nl,vl;
		// try parsing that now	
		while((ret=PSYC_parseHeader(
					&cursor,
					buffer,index,
					&varname,&nl,
					&varvalue,&vl)) == 0)
		{
			write(0,varname,nl);
			write(0," = ",3);
			write(0,varvalue,vl);
			write(0,"\n",1);
		}
		printf("ret0: %d\ncursor0: %d\n",
				ret,cursor);

		if(ret == 3 || ret == 2)
		{
			write(0,varname,nl);
			write(0," = ",3);
			write(0,varvalue,vl);
			write(0,"\n",1);
		}
		if(ret ==2) // header finished
		while((ret=PSYC_parseOpenBody(
						&cursor,
						buffer,index,
						&varname,&nl,
						&varvalue,&vl)) == 0)
		{
			write(0,varname,nl);
			write(0," = ",3);
			write(0,varvalue,vl);
			write(0,"\n",1);
		}

		if(ret == 3 || ret == 2)
		{
			write(0,varname,nl);
			write(0," = ",3);
			write(0,varvalue,vl);
			write(0,"\n",1);
		}

		if(tmp != cursor)
			tmp=cursor;
		else
			return 1;
		printf("ret: %d\ncursor: %d\n--------\n",
				ret,cursor);
	}
	return 0;

}
