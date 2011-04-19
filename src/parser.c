#include <stdint.h>
#include <stdlib.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#include <psyc/parser.h>

#define ADVANCE_CURSOR_OR_RETURN()							 \
	if (++(state->cursor) >= state->buffer.length) \
	{																							 \
		state->cursor = state->startc;							 \
		return PSYC_INSUFFICIENT;										 \
	}

/** @brief isGlyph
 *
 * @todo: document this function
 */
inline char isGlyph(uint8_t g)
{
	switch(g)
	{
		case ':':
		case '=':
		case '?':
		case '+':
		case '-':
			return 1;
		default:
			return 0;
	}

}

inline char isNumeric(uint8_t c)
{
	return c >= '0' && c <= '9';
}

inline char isAlphaNumeric(uint8_t c)
{
	return
		(c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		isNumeric(c);
}

inline char isKwChar(uint8_t c)
{
	return c == '_' || isAlphaNumeric(c);
}

/**
 * @brief Parse variable name or method name.
 * @details It should contain one or more keyword characters.
 * @return PSYC_ERROR or PSYC_SUCCESS
 */
inline int PSYC_parseName(PSYC_State* state, PSYC_Array* name)
{
	name->ptr = state->buffer.ptr + state->cursor;
	name->length = 0;

	while (isKwChar(state->buffer.ptr[state->cursor]))
	{
		name->length++; // was a valid char, increase length
		ADVANCE_CURSOR_OR_RETURN();
	}

	return name->length > 0 ? PSYC_SUCCESS : PSYC_ERROR;
}

/**
 * @brief Parse binary data into value.
 * @details length is the expected length of the data, parsed is the number of bytes parsed so far
 * @return PSYC_COMPLETE or PSYC_INCOMPLETE
 */
inline int PSYC_parseBinaryValue(PSYC_State* state, PSYC_Array* value, unsigned int* length, unsigned int* parsed)
{
	unsigned int remaining = *length - *parsed;
	value->ptr = state->buffer.ptr + state->cursor;

	if (state->cursor + remaining > state->buffer.length) // is the length larger than this buffer?
	{
		value->length = state->buffer.length - state->cursor;
		*parsed += value->length;
		state->cursor = 0;
		return PSYC_INCOMPLETE;
	}

	value->length += remaining;
	state->cursor += remaining;
	*parsed += value->length;

	return PSYC_COMPLETE;
}

/**
 * @brief Parse simple or binary variable
 * @return PSYC_ERROR or PSYC_SUCCESS
 */
inline int PSYC_parseVar(PSYC_State* state, uint8_t* modifier, PSYC_Array* name, PSYC_Array* value)
{
	*modifier = *(state->buffer.ptr + state->cursor);
	ADVANCE_CURSOR_OR_RETURN();

	if (PSYC_parseName(state, name) != PSYC_SUCCESS)
		return PSYC_ERROR_VAR_NAME;

	value->length = 0;

	/* Parse the value.
	 * If we're in the content part check if it's a binary var */
	if (state->part == PSYC_PART_CONTENT && state->buffer.ptr[state->cursor] == ' ') // binary arg
	{ // after SP the length follows.
		ADVANCE_CURSOR_OR_RETURN();
		state->valueLength = 0;
		state->valueParsed = 0;

		if (isNumeric(state->buffer.ptr[state->cursor]))
		{
			do
			{
				state->valueLength = 10 * state->valueLength + state->buffer.ptr[state->cursor] - '0';
				ADVANCE_CURSOR_OR_RETURN();
			}
			while (isNumeric(state->buffer.ptr[state->cursor]));
		}
		else
			return PSYC_ERROR_VAR_LEN;

		// after the length a TAB follows
		if (state->buffer.ptr[state->cursor] != '\t')
			return PSYC_ERROR_VAR_TAB;

		if (state->buffer.length <= ++(state->cursor)) // incremented cursor inside length?
		{
			state->cursor = 0;
			return PSYC_ENTITY_INCOMPLETE;
		}

		if (PSYC_parseBinaryValue(state, value, &(state->valueLength), &(state->valueParsed)) == PSYC_INCOMPLETE)
			return PSYC_ENTITY_INCOMPLETE;

		state->cursor++;
		return PSYC_SUCCESS;
	}
	else if (state->buffer.ptr[state->cursor] == '\t') // simple arg
	{
		ADVANCE_CURSOR_OR_RETURN();
		value->ptr = state->buffer.ptr + state->cursor;

		while (state->buffer.ptr[state->cursor] != '\n')
		{
			value->length++;
			ADVANCE_CURSOR_OR_RETURN();
		}
		state->cursor++;
		return PSYC_SUCCESS;
	}
	else
		return PSYC_ERROR_VAR_TAB;
}

/** @brief generalized line-based parser */
int PSYC_parse(PSYC_State* state, uint8_t* modifier, PSYC_Array* name, PSYC_Array* value)
{
	int ret; /* a return value */
	unsigned int pos;	/* a cursor position */

	/* start position of the current line in the buffer
	 * in case we return insufficent, we rewind to this position */
	state->startc = state->cursor;

	/* first we test if we can access the first char */
	if (state->buffer.length <= state->cursor) // cursor is not inside the length
		return PSYC_INSUFFICIENT; // return insufficient data.

	switch (state->part)
	{
		case PSYC_PART_RESET: // new packet starts here, reset state
			state->valueParsed = 0;
			state->valueLength = 0;
			state->contentParsed = 0;
			state->contentLength = 0;
			state->contentLengthFound = 0;
			state->part = PSYC_PART_HEADER;
			// fall thru

		case PSYC_PART_HEADER:
			/* Each line of the header starts with a glyph,
			 * i.e. :_name, -_name +_name etc,
			 * so just test if the first char is a glyph.
			 */
			if (isGlyph(state->buffer.ptr[state->cursor])) // is the first char a glyph?
			{ // it is a glyph, so a variable starts here
				ret = PSYC_parseVar(state, modifier, name, value);
				return ret == PSYC_SUCCESS ? PSYC_ROUTING : ret;
			}
			else // not a glyph
			{
				state->part = PSYC_PART_LENGTH;
				// fall thru
			}

		case PSYC_PART_LENGTH:
			/* End of header, content starts with an optional length then a NL */
			if (isNumeric(state->buffer.ptr[state->cursor]))
			{
				state->contentLengthFound = 1;
				state->contentLength = 0;
				do
				{
					state->contentLength = 10 * state->contentLength + state->buffer.ptr[state->cursor] - '0';
					ADVANCE_CURSOR_OR_RETURN();
				}
				while (isNumeric(state->buffer.ptr[state->cursor]));
			}

			if (state->buffer.ptr[state->cursor] == '\n') /* start of content */
			{
				/* If we need to parse the header only and we know the content length, then skip content parsing. */
				if (state->flags & PSYC_HEADER_ONLY && state->contentLengthFound)
					state->part = PSYC_PART_DATA;
				else
					state->part = PSYC_PART_CONTENT;
			}
			else /* not start of content, this must be the end */
			{
				/* if we have a length then it should've been followed by a \n */
				if (state->contentLengthFound)
					return PSYC_ERROR_LENGTH;

				state->part = PSYC_PART_END;
				goto PSYC_PART_END;
			}

			ADVANCE_CURSOR_OR_RETURN();
			state->startc = state->cursor;
			// fall thru

		case PSYC_PART_CONTENT:
			/* In case of an incomplete binary variable resume parsing it. */
			if (state->valueParsed < state->valueLength) {
				ret = PSYC_parseBinaryValue(state, value, &(state->valueLength), &(state->valueParsed));
				state->contentParsed += value->length;
				return ret == PSYC_COMPLETE ? PSYC_ENTITY : PSYC_ENTITY_INCOMPLETE;
			}

			/* Each line of the header starts with a glyph,
			 * i.e. :_name, -_name +_name etc.
			 * So just test if the first char is a glyph.
			 * In the body, the same applies, only that the
			 * method does not start with a glyph.
			 */
			if (isGlyph(state->buffer.ptr[state->cursor]))
			{
				pos = state->cursor;
				ret = PSYC_parseVar(state, modifier, name, value);
				state->contentParsed += state->cursor - pos;
				return ret == PSYC_SUCCESS ? PSYC_ENTITY : ret;
			}
			else
			{
				state->part = PSYC_PART_METHOD;
				state->startc = state->cursor;
				// fall thru
			}

		case PSYC_PART_METHOD:
			pos = state->cursor;
			if (PSYC_parseName(state, name) == PSYC_SUCCESS)
			{ /* the method ends with a \n then the data follows */
				if (state->buffer.ptr[state->cursor] != '\n')
					return PSYC_ERROR_METHOD;

				state->cursor++;
				state->startc = state->cursor;
				state->contentParsed += state->cursor - pos;
				state->part = PSYC_PART_DATA;
				if (state->cursor >= state->buffer.length)
					return PSYC_INSUFFICIENT;
				// fall thru
			}
			else /* no method, which means the packet should end now */
			{
				state->part = PSYC_PART_END;
				state->startc = state->cursor;
				goto PSYC_PART_END;
			}

		case PSYC_PART_DATA:
			value->ptr = state->buffer.ptr + state->cursor;
			value->length = 0;

			if (state->contentLengthFound) // we know the length of the packet
			{
				if (PSYC_parseBinaryValue(state, value, &(state->contentLength), &(state->contentParsed)) == PSYC_INCOMPLETE)
					return PSYC_BODY_INCOMPLETE;

				state->cursor++;
				state->part = PSYC_PART_END;
				return PSYC_BODY;
			}
			else // search for the terminator
			{
				while (1)
				{
					if (state->buffer.ptr[state->cursor] == '\n')
					{
						if (state->cursor+2 >= state->buffer.length) // incremented cursor inside length?
						{
							state->cursor = state->startc; // set to start value
							return PSYC_INSUFFICIENT; // return insufficient
						}

						if (state->buffer.ptr[state->cursor+1] == '|' &&
								state->buffer.ptr[state->cursor+2] == '\n') /* packet ends here */
						{
							state->cursor++;
							state->part = PSYC_PART_END;
							return PSYC_BODY;
						}
					}
					value->length++;
					ADVANCE_CURSOR_OR_RETURN();
				}
			}

		case PSYC_PART_END:
		PSYC_PART_END:
			/* end of packet, at this point we have already passed a \n
				 and the cursor should point to | */
			if (state->cursor+1 >= state->buffer.length) // incremented cursor inside length?
				return PSYC_INSUFFICIENT; // return insufficient

			if (state->buffer.ptr[state->cursor] == '|' &&
					state->buffer.ptr[state->cursor+1] == '\n') /* packet ends here */
			{
				state->cursor += 2;
				state->part = PSYC_PART_RESET;
				return PSYC_COMPLETE;
			}
			else /* packet should've ended here, return error */
			{
				state->part = PSYC_PART_RESET;
				return PSYC_ERROR_END;
			}
	}
	return PSYC_ERROR; // should not be reached
}
