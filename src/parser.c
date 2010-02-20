#include <stdint.h>
#include <stdlib.h>

#ifdef DEBUG
#include <stdio.h>
#endif



/** @brief isGlyph
 *
 * @todo: document this function
 */
inline char isGlyph(uint8_t g)
{
	switch(g)
	{
		case ':':
		case '-':
		case '+':
		case '=':
			return 1;
		default:
			return 0;
	}

}


inline char isNumeric(uint8_t c)
{
	return c >= '0' && c <= '9' ;
}

inline char isAlphaNumeric(uint8_t c)
{
	return
		(( c >= 'a' && c <= 'z' )||
		 ( c >= 'A' && c <= 'Z' )||
			 isNumeric(c))
		;

}


inline int PSYC_parseElement(
		unsigned int* cursor,
		const uint8_t * data, unsigned int dlength,
		const uint8_t** name, unsigned int *nlength,
		const uint8_t** value, unsigned int *vlength,
		char inHeader,char complete);

/** @brief parses a routerVariable
 * 
 *  This function parses one routing variable,
 *  advances the cursor after the variable,
 *  writes the variables name, value and their
 *  lengths in the corresponding out parameters
 *  and returns 0 or an errorcode.
 *
 *  
 *  
 *
 *  @param data pointer to the packet data
 *  @param dlength length of the data (amount of bytes)
 *  @param cursor pointer to the current parsing position.
 *         should be initialized to 0.
 *  @param name pointer-pointer, used to return the position
 *         of the name string
 *  @param nlength pointer to which the length of 
 *         the name string will be written
 *  @param value pointer-pointer, used to retrun the position
 *         of the value string
 *  @param vlength pointer to which the length of 
 *         the value string will be written 
 *
 *  @returns 0 on success
 *           1 on insufficient data.
 *             This does not advance
 *             the cursor.             
 *           2 header is now finished.
 *             no more header variables follow
 *           3 the packet is complete.
 *           >3 on a context error,
 *           <0 on a parsing error.
 *              This invalidates all but the cursor
 *              out paramater. */
inline int PSYC_parseHeader(
		unsigned int* cursor,
		const uint8_t * data, unsigned int dlength,
		const uint8_t** name, unsigned int *nlength,
		const uint8_t** value, unsigned int *vlength)
{
	return PSYC_parseElement(cursor,data,dlength,name,nlength,value,vlength,1,0); 
}




/** @brief parses one bodyelement
 *
 * This function is nearly identical to 
 * its brother parseClosedBody. *
 *
 * It assumes that we don’t know the
 * real length of the packet and thus
 * searches for the terminator.
 */
inline int PSYC_parseOpenBody(
		unsigned int* cursor,
		const uint8_t * data, unsigned int dlength,
		const uint8_t** name, unsigned int *nlength,
		const uint8_t** value, unsigned int *vlength)
{

	return PSYC_parseElement(cursor,data,dlength,name,nlength,value,vlength,0,0); 
}


/** @brief parses one bodyelement
 *
 *  This parses one body element, that is
 *  either an entity-variable or the method
 *
 *  The function assumes that dlength is set
 *  to the exact length of the packet
 *  so that data[dlength-1]  would be the
 *  ending "\n" of the packet.
 *
 *  The parameters are nearly the same as for
 *  PSYC_parseHeader, only difference is
 *  that a returnvalue of 2 means, we encountered
 *  the method. 
 *  This means that the out paramterer
 *  name contains the methodname and 
 *  value the content.
 *  */
int PSYC_parseClosedBody(
		unsigned int* cursor,
		const uint8_t * data, unsigned int dlength,
		const uint8_t** name, unsigned int *nlength,
		const uint8_t** value, unsigned int *vlength)
{
	return PSYC_parseElement(cursor,data,dlength,name,nlength,value,vlength,0,1); 
}







/** @brief generalized linebased parser */
inline int PSYC_parseElement(
		unsigned int* cursor,
		const uint8_t * data, unsigned int dlength,
		const uint8_t** name, unsigned int *nlength,
		const uint8_t** value, unsigned int *vlength,
		char inHeader,char complete)
{
	/* first we test if we can access the first char */
	if(dlength<=*cursor) // cursor is not inside the length
		return 1; // return insufficient data.

	// in case we return insufficent, we rewind to the start.
	unsigned int startc=*cursor; 

	/* each line of the header starts with a glyph.
	 * iE :_name, -_name +_name etc, so just test if 
	 * the first char is a glyph. */
	if(1==inHeader)
	{
		if(!isGlyph(data[*cursor])) // is the first char not a glyph?
		{
			// the only other possibility now is that the packet 
			// is complete(empty packet) or that the method started.
			if(isAlphaNumeric(data[*cursor]))
				return 2; // return header finished

			if(data[*cursor] == '|')
			{
				if(dlength<=++(*cursor)) // incremented cursor inside lenght?
				{
					*cursor=startc; // set to start value
					return 1; // return insufficient
				}

				if(data[*cursor]=='\n')
				{
					++(*cursor);
					return 3; // return packet finished
				}
			}
			return -6; // report error
		}else // it is a glyph, so a variable name starts here
		{
			*name = data+*cursor;
			*nlength = 1;
		}
	}
	char method=0;
	/* in the body, the same applies, only that the
	 * method does not start with a glyph.*/ 
	if(0==inHeader && !isGlyph(data[*cursor]))
	{
		if(!isAlphaNumeric(data[*cursor]) && data[*cursor] != '_')
		{
			// the body rule is optional, which means
			// that now also just |\n can follow.
			if(data[*cursor] == '|')
			{
				if(dlength<=++(*cursor)) // incremented cursor inside lenght?
				{
					*cursor=startc; // set to start value
					return 1; // return insufficient
				}

				if(data[*cursor]=='\n')
				{
					++(*cursor);
					return 3; // return packet finished
				}
			}
			return -5; // report error
		}
		else
			method=1;
	}
	else
	{
		*name = data+*cursor;
		*nlength=1;
	}

	/* validate the incremented cursor */
	if(dlength <= ++(*cursor))
	{
		*cursor=startc;
		return 1;
	}

	/* what follows is the name. At least one char.
	 * allowed is alphanumeric and _ */	 

	// first char must exist.
	if(!isAlphaNumeric(data[*cursor]) // is it not alphanum
			&& data[*cursor] != '_')  // AND not '_'. must be invalid then	
		return -1; // return parser error.

	if(1 == method) // for the method..
	{
		*name=data+*cursor; // the name starts here.
		*nlength=1; // the initial length is 1.
	}else
		*nlength+=1;

	/* now checking how long the name of the variable is. */
	unsigned int i=0;
	while(1)
	{
		if(dlength<= ++(*cursor)) // incremented cursor inside lenght?
		{
			*cursor=startc; // set to start value
			return 1; // return insufficient
		}

		// same as bevore
		if(!isAlphaNumeric(data[*cursor]) &&
				data[*cursor] != '_')					
			break; // not valid? then stop the loop right here

		++(*nlength); // was a valid char, increase length

	}

	/* we now parsed the variable name successfully
	 * after the name either a \n or a \t follows.
	 *
	 * for the method, the data starts after an \n
	 * so checking for \n too here

	 * We dont check if cursor inside length, because
	 * the last loop iteration did that already. 
	 */
	if(data[*cursor] == '\t' || data[*cursor] == '\n') // && method==1)) 
	{
		/* after the \t the arg-data follows, which is
		 * anything but \n. arg-data can be of length 0
		 *
		 * for the method: after the \n data follows,
		 * which is anything but \n|\n
		 *
		 * arg-data=*value. we set value here so it
		 * points to a valid range and so we point
		 * to the first potential arg-data byte.
		 * If there is no arg-data, we still have
		 * the length attribute on 0.  */
		if(method == 0 && data[*cursor] == '\n') // emptyvar
		{
			*value=data+*cursor;
			*vlength=0;
		}else
		{
			*value=data+*cursor+1; 
			if(1== complete) // we know the length of the packet
			{
				*vlength= dlength - *cursor -3;
			}else
			{
				*vlength=0;
				while(1)
				{
					if(dlength<=++(*cursor)) // incremented cursor inside lenght?
					{
						*cursor=startc; // set to start value
						return 1; // return insufficient
					}

					if(0 == method && data[*cursor] == '\n')
						break;

					if(1 == method && data[*cursor] == '|')
					{
						if(dlength<=++(*cursor)) // incremented cursor inside lenght?
						{
							*cursor=startc; // set to start value
							return 1; // return insufficient
						}

						/* packet finishes here */
						if(data[*cursor] == '\n')
						{	
							*cursor+=1;
							return 3;
						}
					}
					++(*vlength); 
				}}
		}
	}else if(inHeader == 0 && method==0 && data[*cursor] == ' ') // oi, its a binary var!
	{ // after SP the length follows.
		const uint8_t * bin_length_str = data + *cursor+1;
		int strln = 0;
		do
		{
			if(dlength<=++(*cursor)) // incremented cursor inside lenght?
			{
				*cursor=startc; // set to start value
				return 1; // return insufficient
			}

			++strln;

		}while(isNumeric(data[*cursor]));
		// after the length a TAB follows
		if(data[*cursor] != '\t')
			return -8;

		// now we have the length. convert it to int
		int binLength = atoi(bin_length_str);
		// is that still in this buffer?
		if(dlength <= *cursor+binLength+1 )
		{
			*cursor=startc;
			return 1;
		}

		*value = data + *cursor+1;		
		*vlength=binLength;
		*cursor += binLength+1;
	}else
		return -8;




	/* if there was a \t, then we parsed up until the 
	 * \n char from the simple-arg rule ( \t arg-data \n )
	 *
	 * Now, if there would be no \t, we still would be at
	 * the point where a \n must follow.
	 *
	 * So, just checking \n here will cover both cases of
	 * the alternative ( simple-arg / \n ) from rule
	 * routing-modifier 
	 *
	 * again, the loop has already validated the cursors
	 * position*/
	if(data[*cursor] != '\n')
		return -2; // return parser error.

	/* if a \n follows now, the an body is attached.
	 * if not, a |\n must follow */
	if(dlength<=++(*cursor)) // incremented cursor inside lenght?
	{
		*cursor=startc; // set to start value
		return 1; // return insufficient
	}

	if(1 == inHeader && data[*cursor] == '\n') 
	{
		*cursor+=1;
		return 2; // line is complete, but body starts now.
	}

	if(data[*cursor] != '|') // no pipe, then only line complete, not the packet.
		return 0; 

	if(dlength<=++(*cursor)) // incremented cursor inside lenght?
	{
		*cursor=startc; // set to start value
		return 1; // return insufficient
	}
	if(data[*cursor] != '\n')	
		return -4;

	*cursor+=1;
	return 3; // packet is complete


}

/** @brief parses one variable in two buffers
 *
 *  This function is nearly identical to its
 *  brother parseHeader. The difference is,
 *  it uses two buffers and return parameters
 *  for everything. It is meant to be used 
 *  in case parseHeader returned 2 for 
 *  insufficient data and you don’t
 *  like to copy memory around to 
 *  have all the data in one buffer.
 *  Using this function, you can pass two
 *  data buffers. The consequence is, 
 *  that name and value can be distributed
 *  on two different buffers and thus need
 *  also two out paramaters. If only one 
 *  will be used, length of the second 
 *  will be 0.
 *
 *  If your data is spread over more
 *  than two buffers, you need to
 *  copy that in one or two buffers.
 *  Given the unlikleyness of that
 *  event, we don't offer a three
 *  or more buffer function here.
 *
 */
int PSYC_parseHeader2(
		unsigned int* cursor,
		const uint8_t * data1, unsigned int dlength1,
		const uint8_t * data2, unsigned int dlength2,
		const uint8_t** name1, unsigned int *nlength1,
		const uint8_t** name2, unsigned int *nlength2,
		const uint8_t** value1, unsigned int *vlength1,
		const uint8_t** value2, unsigned int *vlength2);


/* @brief parses an bodyelement in two buffers
 *
 * This function is the brother of parseHeader2.
 * It behaives the same as
 * parseOpenBody and parseHeader2. */
int PSYC_parseOpenBody2(
		unsigned int* cursor,
		const uint8_t * data1, unsigned int dlength1,
		const uint8_t * data2, unsigned int dlength2,
		const uint8_t** name1, unsigned int *nlength1,
		const uint8_t** name2, unsigned int *nlength2,
		const uint8_t** value1, unsigned int *vlength1,
		const uint8_t** value2, unsigned int *vlength2);




