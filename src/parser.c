#include <stdint.h>
#include <stdlib.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#include <psyc/parser.h>

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



/** @brief generalized linebased parser */
inline int PSYC_parse(
		PSYC_State* state,
		PSYC_Array* name, PSYC_Array* value,
		uint8_t* modifier, unsigned long* expectedBytes)
{
	/* first we test if we can access the first char */
	if(state->buffer.length<=state->cursor) // cursor is not inside the length
		return 1; // return insufficient data.

	// in case we return insufficent, we rewind to the start.
	unsigned int startc=state->cursor; 

	/* each line of the header starts with a glyph.
	 * iE :_name, -_name +_name etc, so just test if 
	 * the first char is a glyph. */
	if(1==state->inHeader)
	{
		if(!isGlyph(state->buffer.ptr[state->cursor])) // is the first char not a glyph?
		{
			// the only other possibility now is that the packet 
			// is complete(empty packet) or that the method started.
			if(isAlphaNumeric(state->buffer.ptr[state->cursor]))
			{
				state->inHeader = 0;
				return 2; // return header finished
			}

			if(state->buffer.ptr[state->cursor] == '|')
			{
				if(state->buffer.length<=++(state->cursor)) // incremented cursor inside lenght?
				{
					state->cursor=startc; // set to start value
					return 1; // return insufficient
				}

				if(state->buffer.ptr[state->cursor]=='\n')
				{
					++(state->cursor);
					return 3; // return packet finished
				}
			}
			return -6; // report error
		}else // it is a glyph, so a variable name starts here
		{
			*modifier = *(state->buffer.ptr+state->cursor);
			
			if (state->buffer.length <= ++(state->cursor))
			{
				state->cursor = startc; // rewind to start of line
				return 1;  // return insufficient
			}

			name->ptr = state->buffer.ptr + state->cursor;

			name->length = 0;
		}
	}

	char method=0;
	/* in the body, the same applies, only that the
	 * method does not start with a glyph.*/ 
	if(0==state->inHeader && !isGlyph(state->buffer.ptr[state->cursor]))
	{
		if(!isAlphaNumeric(state->buffer.ptr[state->cursor]) && state->buffer.ptr[state->cursor] != '_')
		{
			// the body rule is optional, which means
			// that now also just |\n can follow.
			if(state->buffer.ptr[state->cursor] == '|')
			{
				if(state->buffer.length<=++(state->cursor)) // incremented cursor inside lenght?
				{
					state->cursor=startc; // set to start value
					return 1; // return insufficient
				}

				if(state->buffer.ptr[state->cursor]=='\n')
				{
					++(state->cursor);
					return 3; // return packet finished
				}
			}
			return -5; // report error
		}
		else
		{
			name->ptr = state->buffer.ptr+state->cursor;
			name->length=1;
			method=1;
		}
	}
	else
	{
		*modifier = *(state->buffer.ptr+state->cursor);
		
		if (state->buffer.length <= ++(state->cursor))
		{
			state->cursor = startc; // rewind
			return 1; // return insufficient
		}

		name->ptr = state->buffer.ptr+state->cursor;
		name->length=1;
	}

	/* validate the incremented cursor */
	if(state->buffer.length <= ++(state->cursor))
	{
		state->cursor=startc;
		return 1;
	}

	/* what follows is the name. At least one char.
	 * allowed is alphanumeric and _ */	 

	// first char must exist.
	if(!isAlphaNumeric(state->buffer.ptr[state->cursor]) // is it not alphanum
			&& state->buffer.ptr[state->cursor] != '_')  // AND not '_'. must be invalid then	
		return -1; // return parser error.

	name->length+=1;

	/* now checking how long the name of the variable is. */
	unsigned int i=0;
	while(1)
	{
		if(state->buffer.length<= ++(state->cursor)) // incremented cursor inside lenght?
		{
			state->cursor=startc; // set to start value
			return 1; // return insufficient
		}

		// same as bevore
		if(!isAlphaNumeric(state->buffer.ptr[state->cursor]) &&
				state->buffer.ptr[state->cursor] != '_')					
			break; // not valid? then stop the loop right here

		++(name->length); // was a valid char, increase length
	}

	/* we now parsed the variable name successfully
	 * after the name either a \n or a \t follows.
	 *
	 * for the method, the state->buffer.ptr starts after an \n
	 * so checking for \n too here

	 * We dont check if cursor inside length, because
	 * the last loop iteration did that already. 
	 */
	if(state->buffer.ptr[state->cursor] == '\t' || state->buffer.ptr[state->cursor] == '\n') // && method==1)) 
	{
		/* after the \t the arg-state->buffer.ptr follows, which is
		 * anything but \n. arg-state->buffer.ptr can be of length 0
		 *
		 * for the method: after the \n state->buffer.ptr follows,
		 * which is anything but \n|\n
		 *
		 * but state->buffer.ptr is optional, so we first check 
		 * here if state->buffer.ptr follows at all.
		 *
		 * arg-state->buffer.ptr=value->ptr. we set value here so it
		 * points to a valid range and so we point
		 * to the first potential arg-state->buffer.ptr byte.
		 * If there is no arg-state->buffer.ptr, we still have
		 * the length attribute on 0.  */
		if((method == 0 && state->buffer.ptr[state->cursor] == '\n') /* emptyvar */ ||
			(method == 1 && state->cursor+2 < state->buffer.length && 
			 state->buffer.ptr[state->cursor+1] == '|' && 
			 state->buffer.ptr[state->cursor+2] == '\n') /*no state->buffer.ptr */ )
		{
			value->ptr=state->buffer.ptr+state->cursor;
			value->length=0;
		}
		else
		{
			value->ptr=state->buffer.ptr+state->cursor+1; 
			if (1==state->length) // we know the length of the packet
			{
				value->length= state->buffer.length - state->cursor -3;
			}
			else // else search for the terminator
			{
				value->length=0;

				while (1)
				{
					if(state->buffer.length<=++(state->cursor)) // incremented cursor inside lenght?
					{
						state->cursor=startc; // set to start value
						return 1; // return insufficient
					}

					if(0 == method && state->buffer.ptr[state->cursor] == '\n')
						break;

					if(1 == method && state->buffer.ptr[state->cursor] == '\n')
					{
						if(state->buffer.length<=(state->cursor)+2) // incremented cursor inside lenght?
						{
							state->cursor=startc; // set to start value
							return 1; // return insufficient
						}

						if(state->buffer.ptr[state->cursor+1] == '|')
							if(state->buffer.ptr[state->cursor+2] == '\n')
							{
								/* packet finishes here */
								state->cursor+=3;
								return 3;
							}
							
					}
					++(value->length); 
				}
			}
		}
	}else if(state->inHeader == 0 && method==0 && state->buffer.ptr[state->cursor] == ' ') // oi, its a binary var!
	{ // after SP the length follows.
		unsigned int binLength= 0;

		if(state->buffer.length<=++(state->cursor)) // incremented cursor inside lenght?
		{
			state->cursor=startc; // set to start value
			return 1; // return insufficient
		}

		while(isNumeric(state->buffer.ptr[state->cursor]));
		{
			binLength = 10 * binLength + state->buffer.ptr[state->cursor] - '0';

			if(state->buffer.length<=++(state->cursor)) // incremented cursor inside lenght?
			{
				state->cursor=startc; // set to start value
				return 1; // return insufficient
			}
		}
		// after the length a TAB follows
		if (state->buffer.ptr[state->cursor] != '\t')
			return -8;

		// is the length still in this buffer?
		if(state->buffer.length <= state->cursor+binLength+1 )
		{
			state->cursor=startc;
			return 1;
		}

		value->ptr = state->buffer.ptr + state->cursor+1;		
		value->length=binLength;
		state->cursor += binLength+1;
	}
	else
		return -8;




	/* if there was a \t, then we parsed up until the 
	 * \n char from the simple-arg rule ( \t arg-state->buffer.ptr \n )
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
	if(state->buffer.ptr[state->cursor] != '\n')
		return -2; // return parser error.

	/* if a \n follows now, the an body is attached.
	 * if not, a |\n must follow */
	if(state->buffer.length<=++(state->cursor)) // incremented cursor inside lenght?
	{
		state->cursor=startc; // set to start value
		return 1; // return insufficient
	}

	if(1 == state->inHeader && state->buffer.ptr[state->cursor] == '\n') 
	{
		state->cursor+=1;
		state->inHeader = 0;
		return 2; // line is complete, but body starts now.
	}

	if(state->buffer.ptr[state->cursor] != '|') // no pipe, then only line complete, not the packet.
		return 0; 

	if(state->buffer.length<=++(state->cursor)) // incremented cursor inside lenght?
	{
		state->cursor=startc; // set to start value
		return 1; // return insufficient
	}
	if(state->buffer.ptr[state->cursor] != '\n')	
		return -4;

	state->cursor+=1;
	return 3; // packet is complete
}

