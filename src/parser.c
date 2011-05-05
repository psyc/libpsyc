#include <stdint.h>
#include <stdlib.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#include <psyc/lib.h>
#include <psyc/parser.h>

#define ADVANCE_CURSOR_OR_RETURN(ret)            \
	if (++(state->cursor) >= state->buffer.length) \
	{                                              \
		state->cursor = state->startc;               \
		return ret;                                  \
	}

/** 
 * Determines if the argument is a glyph.
 * Glyphs are: : = + - ? !
 */
static inline
char isGlyph (uint8_t g)
{
	switch(g)
	{
		case ':':
		case '=':
		case '+':
		case '-':
		case '?':
		case '!':
			return 1;
		default:
			return 0;
	}
}

/**
 * Determines if the argument is numeric.
 */
static inline
char isNumeric (uint8_t c)
{
	return c >= '0' && c <= '9';
}

/**
 * Determines if the argument is alphanumeric.
 */
static inline
char isAlphaNumeric (uint8_t c)
{
	return
		(c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		isNumeric(c);
}

/**
 * Determines if the argument is a keyword character.
 * Keyword characters are: alphanumeric and _
 */
static inline
char isKwChar (uint8_t c)
{
	return isAlphaNumeric(c) || c == '_';
}

/**
 * Parse variable name or method name.
 * It should contain one or more keyword characters.
 * @return PSYC_PARSE_ERROR or PSYC_PARSE_SUCCESS
 */
static inline
psycParseRC psyc_parseKeyword (psycParseState *state, psycString *name)
{
	name->ptr = state->buffer.ptr + state->cursor;
	name->length = 0;

	while (isKwChar(state->buffer.ptr[state->cursor]))
	{
		name->length++; // was a valid char, increase length
		ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
	}

	return name->length > 0 ? PSYC_PARSE_SUCCESS : PSYC_PARSE_ERROR;
}

/**
 * Parse binary data.
 *
 * @param state Parser state.
 * @param value Start & length of parsed data is saved here.
 * @param length Expected length of the data.
 * @param parsed Number of bytes parsed so far.
 *
 * @return PSYC_PARSE_COMPLETE or PSYC_PARSE_INCOMPLETE
 */
static inline
psycParseRC psyc_parseBinaryValue (psycParseState *state, psycString *value,
                                   size_t *length, size_t *parsed)
{
	size_t remaining = *length - *parsed;
	value->ptr = state->buffer.ptr + state->cursor;

	if (state->cursor + remaining > state->buffer.length)
	{ // value doesn't fit in the buffer completely
		value->length = state->buffer.length - state->cursor;
		state->cursor += value->length;
		*parsed += value->length;
		return PSYC_PARSE_INCOMPLETE;
	}

	value->length = remaining;
	state->cursor += remaining;
	*parsed += remaining;
	assert(*parsed == *length);

	return PSYC_PARSE_COMPLETE;
}

/**
 * Parse simple or binary variable.
 * @return PSYC_PARSE_ERROR or PSYC_PARSE_SUCCESS
 */
static inline
psycParseRC psyc_parseModifier (psycParseState *state, char *oper,
                                psycString *name, psycString *value)
{
	*oper = *(state->buffer.ptr + state->cursor);
	ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);

	psycParseRC ret = psyc_parseKeyword(state, name);
	if (ret == PSYC_PARSE_ERROR)
		return PSYC_PARSE_ERROR_MOD_NAME;
	else if (ret != PSYC_PARSE_SUCCESS)
		return ret;

	size_t length = 0;
	value->length = 0;
	state->valueLength = 0;
	state->valueLengthFound = 0;
	state->valueParsed = 0;

	// Parse the value.
	// If we're in the content part check if it's a binary var.
	if (state->part == PSYC_PART_CONTENT && state->buffer.ptr[state->cursor] == ' ') // binary arg
	{ // After SP the length follows.
		ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);

		if (isNumeric(state->buffer.ptr[state->cursor]))
		{
			state->valueLengthFound = 1;
			do
			{
				length = 10 * length + state->buffer.ptr[state->cursor] - '0';
				ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
			}
			while (isNumeric(state->buffer.ptr[state->cursor]));
			state->valueLength = length;
		}
		else
			return PSYC_PARSE_ERROR_MOD_LEN;

		// After the length a TAB follows.
		if (state->buffer.ptr[state->cursor] != '\t')
			return PSYC_PARSE_ERROR_MOD_TAB;

		if (++(state->cursor) >= state->buffer.length)
			return length ? PSYC_PARSE_INCOMPLETE : PSYC_PARSE_SUCCESS; // if length=0 we're done

		ret = psyc_parseBinaryValue(state, value, &(state->valueLength), &(state->valueParsed));
		if (ret == PSYC_PARSE_INCOMPLETE)
			return ret;

		return PSYC_PARSE_SUCCESS;
	}
	else if (state->buffer.ptr[state->cursor] == '\t') // simple arg
	{
		ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
		value->ptr = state->buffer.ptr + state->cursor;

		while (state->buffer.ptr[state->cursor] != '\n')
		{
			value->length++;
			ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
		}

		return PSYC_PARSE_SUCCESS;
	}
	else
		return PSYC_PARSE_ERROR_MOD_TAB;
}

/**
 * Parse PSYC packets.
 * Generalized line-based parser.
 */
psycParseRC psyc_parse (psycParseState *state, char *oper,
                        psycString *name, psycString *value)
{
#ifdef DEBUG
	if (state->flags & PSYC_PARSE_ROUTING_ONLY &&
	    state->flags & PSYC_PARSE_START_AT_CONTENT)
		PP(("Invalid flag combination"))
#endif

	psycParseRC ret; // a return value
	size_t pos = state->cursor;	// a cursor position

	// Start position of the current line in the buffer
	// in case we return insufficent, we rewind to this position.
	state->startc = state->cursor;

	// First we test if we can access the first char.
	if (state->cursor >= state->buffer.length) // cursor is not inside the length
		return PSYC_PARSE_INSUFFICIENT;

	switch (state->part)
	{
		case PSYC_PART_RESET: // New packet starts here, reset state.
			state->valueParsed = 0;
			state->valueLength = 0;
			state->valueLengthFound = 0;
			state->routingLength = 0;
			state->contentParsed = 0;
			state->contentLength = 0;
			state->contentLengthFound = 0;
			state->part = PSYC_PART_ROUTING;
			// fall thru

		case PSYC_PART_ROUTING:
			if (state->routingLength > 0)
			{
				if (state->buffer.ptr[state->cursor] != '\n')
					return PSYC_PARSE_ERROR_MOD_NL;
				ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
			}

			// Each line of the header starts with a glyph,
			// i.e. :_name, -_name +_name etc,
			// so just test if the first char is a glyph.
			if (isGlyph(state->buffer.ptr[state->cursor])) // is the first char a glyph?
			{ // it is a glyph, so a variable starts here
				ret = psyc_parseModifier(state, oper, name, value);
				state->routingLength += state->cursor - pos;
				return ret == PSYC_PARSE_SUCCESS ? PSYC_PARSE_ROUTING : ret;
			}
			else // not a glyph
			{
				state->part = PSYC_PART_LENGTH;
				state->startc = state->cursor;
				// fall thru
			}

		case PSYC_PART_LENGTH:
			// End of header, content starts with an optional length then a NL
			if (isNumeric(state->buffer.ptr[state->cursor]))
			{
				state->contentLengthFound = 1;
				state->contentLength = 0;

				do
				{
					state->contentLength = 10 * state->contentLength + state->buffer.ptr[state->cursor] - '0';
					ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
				}
				while (isNumeric(state->buffer.ptr[state->cursor]));
			}

			if (state->buffer.ptr[state->cursor] == '\n') // start of content
			{
				// If we need to parse the header only and we know the content length,
				// then skip content parsing.
				if (state->flags & PSYC_PARSE_ROUTING_ONLY)
				{
					state->part = PSYC_PART_DATA;
					if (++(state->cursor) >= state->buffer.length)
						return PSYC_PARSE_INSUFFICIENT;
					goto PSYC_PART_DATA;
				}
				else
					state->part = PSYC_PART_CONTENT;
			}
			else // Not start of content, this must be the end.
			{
				// If we have a length then it should've been followed by a \n
				if (state->contentLengthFound)
					return PSYC_PARSE_ERROR_LENGTH;

				state->part = PSYC_PART_END;
				goto PSYC_PART_END;
			}

			state->startc = state->cursor + 1;
			ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
			// fall thru

		case PSYC_PART_CONTENT:
			// In case of an incomplete binary variable resume parsing it.
			if (state->valueParsed < state->valueLength)
			{
				ret = psyc_parseBinaryValue(state, value, &(state->valueLength), &(state->valueParsed));
				state->contentParsed += value->length;

				if (ret == PSYC_PARSE_INCOMPLETE)
					return PSYC_PARSE_ENTITY_INCOMPLETE;

				return PSYC_PARSE_ENTITY;
			}

			pos = state->cursor;

			if (state->contentParsed > 0)
			{
				if (state->buffer.ptr[state->cursor] != '\n')
					return PSYC_PARSE_ERROR_MOD_NL;
				ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
			}

			// Each line of the header starts with a glyph,
			// i.e. :_name, -_name +_name etc.
			// So just test if the first char is a glyph.
			// In the body, the same applies, only that the
			// method does not start with a glyph.
			if (isGlyph(state->buffer.ptr[state->cursor]))
			{
				ret = psyc_parseModifier(state, oper, name, value);
				state->contentParsed += state->cursor - pos;

				if (ret == PSYC_PARSE_INCOMPLETE)
					return PSYC_PARSE_ENTITY_INCOMPLETE;
				else if (ret == PSYC_PARSE_SUCCESS)
					return PSYC_PARSE_ENTITY;

				return ret;
			}
			else
			{
				state->contentParsed += state->cursor - pos;
				state->startc = state->cursor;
				state->part = PSYC_PART_METHOD;
				// fall thru
			}

		case PSYC_PART_METHOD:
			pos = state->cursor;
			ret = psyc_parseKeyword(state, name);

			if (ret == PSYC_PARSE_INSUFFICIENT)
				return ret;
			else if (ret == PSYC_PARSE_SUCCESS)
			{ // the method ends with a \n then the data follows
				if (state->buffer.ptr[state->cursor] != '\n')
					return PSYC_PARSE_ERROR_METHOD;

				state->valueLengthFound = 0;
				state->valueParsed = 0;
				state->valueLength = 0;

				if (state->contentLengthFound)
				{ // if length was found set start position to the beginning of data
					state->cursor++;
					state->startc = state->cursor;
					state->contentParsed += state->cursor - pos;
					state->part = PSYC_PART_DATA;
				}
				else // otherwise keep it at the beginning of method
				{
					ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
				}

				// fall thru
			}
			else // No method, which means the packet should end now.
			{
				state->part = PSYC_PART_END;
				state->startc = state->cursor;
				goto PSYC_PART_END;
			}

		case PSYC_PART_DATA:
		PSYC_PART_DATA:
			value->ptr = state->buffer.ptr + state->cursor;
			value->length = 0;

			if (state->contentLengthFound) // We know the length of the packet.
			{
				if (!state->valueLengthFound) // start of data
				{
					state->valueLengthFound = 1;
					state->valueLength = state->contentLength - state->contentParsed; // length of data
					if (state->valueLength && !(state->flags & PSYC_PARSE_ROUTING_ONLY))
						state->valueLength--; // \n at the end is not part of data
				}
				if (state->valueParsed < state->valueLength)
				{
					ret = psyc_parseBinaryValue(state, value, &(state->valueLength), &(state->valueParsed));
					state->contentParsed += value->length;

					if (ret == PSYC_PARSE_INCOMPLETE)
						return PSYC_PARSE_BODY_INCOMPLETE;
				}

				state->part = PSYC_PART_END;
				return PSYC_PARSE_BODY;
			}
			else // Search for the terminator.
			{
				size_t datac = state->cursor; // start of data
				while (1)
				{
					uint8_t nl = state->buffer.ptr[state->cursor] == '\n';
					// check for |\n if we're at the start of data or we have found a \n
					if (state->cursor == datac || nl)
					{
						if (state->cursor+1+nl >= state->buffer.length) // incremented cursor inside length?
						{
							state->cursor = state->startc;
							return PSYC_PARSE_INSUFFICIENT;
						}

						if (state->buffer.ptr[state->cursor+nl] == '|' &&
								state->buffer.ptr[state->cursor+1+nl] == '\n') // packet ends here
						{
							if (state->flags & PSYC_PARSE_ROUTING_ONLY)
								value->length++;

							state->contentParsed += state->cursor - pos;
							state->cursor += nl;
							state->part = PSYC_PART_END;
							return PSYC_PARSE_BODY;
						}
					}
					value->length++;
					ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
				}
			}

		case PSYC_PART_END:
		PSYC_PART_END:
			if (state->contentLengthFound && state->valueLengthFound && state->valueLength &&
			    !(state->flags & PSYC_PARSE_ROUTING_ONLY))
			{ // if data was not empty next is the \n at the end of data
				state->valueLength = 0;
				state->valueLengthFound = 0;

				if (state->buffer.ptr[state->cursor] != '\n')
					return PSYC_PARSE_ERROR_END;

				state->contentParsed++;
				state->cursor++;
			}

			// End of packet, at this point we have already passed a \n
			// and the cursor should point to |
			if (state->cursor+1 >= state->buffer.length) // incremented cursor inside length?
				return PSYC_PARSE_INSUFFICIENT;

			if (state->buffer.ptr[state->cursor] == '|' &&
			    state->buffer.ptr[state->cursor+1] == '\n') // packet ends here
			{
				state->cursor += 2;
				state->part = PSYC_PART_RESET;
				return PSYC_PARSE_COMPLETE;
			}
			else // packet should've ended here, return error
			{
				state->part = PSYC_PART_RESET;
				return PSYC_PARSE_ERROR_END;
			}
	}
	return PSYC_PARSE_ERROR; // should not be reached
}

/**
 * List value parser.
 * @return see psycListRC.
 */
psycParseListRC psyc_parseList (psycParseListState *state, psycString *name,
                                psycString *value, psycString *elem)
{
	if (state->cursor >= state->buffer.length)
		return PSYC_PARSE_LIST_INCOMPLETE;

	state->startc = state->cursor;

	if (!state->type) // If type is not set we're at the start
	{
		if (name->length < 5 || memcmp(name->ptr, "_list", 5) != 0 ||
		    (name->length > 5 && name->ptr[5] != '_')) // name should be _list or should start with _list_
			return PSYC_PARSE_LIST_ERROR_NAME;

		// First character is either | for text lists, or a number for binary lists
		if (state->buffer.ptr[state->cursor] == '|')
		{
			state->type = PSYC_LIST_TEXT;
			state->cursor++;
		}
		else if (isNumeric(state->buffer.ptr[state->cursor]))
			state->type = PSYC_LIST_BINARY;
		else
			return PSYC_PARSE_LIST_ERROR_TYPE;
	}

	if (state->type == PSYC_LIST_TEXT)
	{
		elem->ptr = state->buffer.ptr + state->cursor;
		elem->length = 0;

		if (state->cursor >= state->buffer.length)
			return PSYC_PARSE_LIST_END;

		while (state->buffer.ptr[state->cursor] != '|')
		{
			elem->length++;
			if (++(state->cursor) >= state->buffer.length)
				return PSYC_PARSE_LIST_END;
		}
		state->cursor++;
		return PSYC_PARSE_LIST_ELEM;
	}
	else // binary list
	{
		if (!(state->elemParsed < state->elemLength))
		{
			// Element starts with a number.
			if (isNumeric(state->buffer.ptr[state->cursor]))
			{
				do
				{
					state->elemLength = 10 * state->elemLength + state->buffer.ptr[state->cursor] - '0';
					ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_LIST_INCOMPLETE);
				}
				while (isNumeric(state->buffer.ptr[state->cursor]));
			}
			else
				return PSYC_PARSE_LIST_ERROR_LEN;

			if (state->buffer.ptr[state->cursor] != ' ')
				return PSYC_PARSE_LIST_ERROR_LEN;

			state->cursor++;
			elem->ptr = state->buffer.ptr + state->cursor;
			elem->length = 0;
			state->elemParsed = 0;
		}

		// Start or resume parsing the binary data
		if (state->elemParsed < state->elemLength)
		{
			if (psyc_parseBinaryValue((psycParseState*)state, elem,
			                          &(state->elemLength), &(state->elemParsed)) == PSYC_PARSE_INCOMPLETE)
				return PSYC_PARSE_LIST_INCOMPLETE;

			state->elemLength = 0;

			if (state->cursor >= state->buffer.length)
				return PSYC_PARSE_LIST_END;

			if (state->buffer.ptr[state->cursor] != '|')
				return PSYC_PARSE_LIST_ERROR_DELIM;

			state->cursor++;
			return PSYC_PARSE_LIST_ELEM;
		}
	}

	return PSYC_PARSE_LIST_ERROR; // should not be reached
}
