#include <stdint.h>
#include <stdlib.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#include "lib.h"
#include <psyc/packet.h>
#include <psyc/parse.h>

#define ADVANCE_CURSOR_OR_RETURN(ret)					\
    if (++(state->cursor) >= state->buffer.length) {			\
	state->cursor = state->startc;					\
	return ret;							\
    }

typedef enum {
    PARSE_ERROR = -1,
    PARSE_SUCCESS = 0,
    PARSE_INSUFFICIENT = 1,
    PARSE_COMPLETE = 100,
    PARSE_INCOMPLETE = 101,
} ParseRC;

/**
 * Parse variable name or method name.
 * It should contain one or more keyword characters.
 * @return PARSE_ERROR or PARSE_SUCCESS
 */
static inline ParseRC
parse_keyword (PsycParseState *state, PsycString *name)
{
    name->data = state->buffer.data + state->cursor;
    name->length = 0;

    while (psyc_is_kw_char(state->buffer.data[state->cursor])) {
	name->length++;	// was a valid char, increase length
	ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
    }

    return name->length > 0 ? PARSE_SUCCESS : PARSE_ERROR;
}

/**
 * Parse binary data.
 *
 * @param state Parser state.
 * @param value Start & length of parsed data is saved here.
 * @param length Expected length of the data.
 * @param parsed Number of bytes parsed so far.
 *
 * @return PARSE_COMPLETE or PARSE_INCOMPLETE
 */
static inline ParseRC
psyc_parse_binary_value (PsycParseState *state, PsycString *value,
			 size_t *length, size_t *parsed)
{
    size_t remaining = *length - *parsed;
    value->data = state->buffer.data + state->cursor;

    if (state->cursor + remaining > state->buffer.length) {
	// value doesn't fit in the buffer completely
	value->length = state->buffer.length - state->cursor;
	state->cursor += value->length;
	*parsed += value->length;
	return PARSE_INCOMPLETE;
    }

    value->length = remaining;
    state->cursor += remaining;
    *parsed += remaining;
    assert(*parsed == *length);

    return PARSE_COMPLETE;
}

/**
 * Parse simple or binary variable.
 * @return PARSE_ERROR or PARSE_SUCCESS
 */
static inline ParseRC
psyc_parse_modifier (PsycParseState *state, char *oper,
		     PsycString *name, PsycString *value)
{
    *oper = *(state->buffer.data + state->cursor);
    ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);

    ParseRC ret = parse_keyword(state, name);
    if (ret == PARSE_ERROR)
	return PSYC_PARSE_ERROR_MOD_NAME;
    else if (ret != PARSE_SUCCESS)
	return ret;

    size_t length = 0;
    value->length = 0;
    state->valuelen = 0;
    state->valuelen_found = 0;
    state->value_parsed = 0;

    // Parse the value.
    // If we're in the content part check if it's a binary var.
    if (state->part == PSYC_PART_CONTENT && state->buffer.data[state->cursor] == ' ') {
	// binary arg
	// After SP the length follows.
	ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);

	if (psyc_is_numeric(state->buffer.data[state->cursor])) {
	    state->valuelen_found = 1;
	    do {
		length = 10 * length + state->buffer.data[state->cursor] - '0';
		ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
	    }
	    while (psyc_is_numeric(state->buffer.data[state->cursor]));
	    state->valuelen = length;
	} else
	    return PSYC_PARSE_ERROR_MOD_LEN;

	// After the length a TAB follows.
	if (state->buffer.data[state->cursor] != '\t')
	    return PSYC_PARSE_ERROR_MOD_TAB;

	if (++(state->cursor) >= state->buffer.length)
	    return length ? PARSE_INCOMPLETE : PARSE_SUCCESS; // if length=0 we're done

	ret =
	    psyc_parse_binary_value(state, value, &(state->valuelen),
				    &(state->value_parsed));
	if (ret == PARSE_INCOMPLETE)
	    return ret;

	return PARSE_SUCCESS;
    } else if (state->buffer.data[state->cursor] == '\t') { // simple arg
	ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
	value->data = state->buffer.data + state->cursor;

	while (state->buffer.data[state->cursor] != '\n') {
	    value->length++;
	    ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
	}

	return PARSE_SUCCESS;
    } else
	return PSYC_PARSE_ERROR_MOD_TAB;
}

/** Parse PSYC packets. */
#ifdef __INLINE_PSYC_PARSE
static inline
#endif
PsycParseRC
psyc_parse (PsycParseState *state, char *oper,
	    PsycString *name, PsycString *value)
{
#ifdef DEBUG
    if (state->flags & PSYC_PARSE_ROUTING_ONLY &&
	state->flags & PSYC_PARSE_START_AT_CONTENT)
	PP(("Invalid flag combination"));
#endif
    ParseRC ret;		// a return value
    size_t pos = state->cursor;	// a cursor position

    // Start position of the current line in the buffer
    // in case we return insufficent, we rewind to this position.
    state->startc = state->cursor;

    if (state->flags & PSYC_PARSE_START_AT_CONTENT
	&& (state->buffer.length == 0 || state->cursor >= state->buffer.length - 1))
	return PSYC_PARSE_COMPLETE;

    // First we test if we can access the first char.
    if (state->cursor >= state->buffer.length) // Cursor is not inside the length.
	return PSYC_PARSE_INSUFFICIENT;

    switch (state->part) {
    case PSYC_PART_RESET: // New packet starts here, reset state.
	state->value_parsed = 0;
	state->valuelen = 0;
	state->valuelen_found = 0;
	state->routinglen = 0;
	state->content_parsed = 0;
	state->contentlen = 0;
	state->contentlen_found = 0;
	state->part = PSYC_PART_ROUTING;
	// fall thru

    case PSYC_PART_ROUTING:
	if (state->routinglen > 0) {
	    if (state->buffer.data[state->cursor] != '\n')
		return PSYC_PARSE_ERROR_MOD_NL;
	    ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
	}
	// Each line of the header starts with a glyph,
	// i.e. :_name, -_name +_name etc,
	// so just test if the first char is a glyph.
	if (psyc_is_glyph(state->buffer.data[state->cursor])) {
	    // it is a glyph, so a variable starts here
	    ret = psyc_parse_modifier(state, oper, name, value);
	    state->routinglen += state->cursor - pos;
	    return ret == PARSE_SUCCESS ? PSYC_PARSE_ROUTING : ret;
	} else { // not a glyph
	    state->part = PSYC_PART_LENGTH;
	    state->startc = state->cursor;
	    // fall thru
	}

    case PSYC_PART_LENGTH:
	// End of header, content starts with an optional length then a NL
	if (psyc_is_numeric(state->buffer.data[state->cursor])) {
	    state->contentlen_found = 1;
	    state->contentlen = 0;

	    do {
		state->contentlen =
		    10 * state->contentlen +
		    state->buffer.data[state->cursor] - '0';
		ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
	    } while (psyc_is_numeric(state->buffer.data[state->cursor]));
	}

	if (state->buffer.data[state->cursor] == '\n') { // start of content
	    // If we need to parse the header only and we know the content length,
	    // then skip content parsing.
	    if (state->flags & PSYC_PARSE_ROUTING_ONLY) {
		state->part = PSYC_PART_DATA;
		if (++(state->cursor) >= state->buffer.length)
		    return PSYC_PARSE_INSUFFICIENT;
		goto PSYC_PART_DATA;
	    } else
		state->part = PSYC_PART_CONTENT;
	} else { // Not start of content, this must be the end.
	    // If we have a length then it should've been followed by a \n
	    if (state->contentlen_found)
		return PSYC_PARSE_ERROR_LENGTH;

	    state->part = PSYC_PART_END;
	    goto PSYC_PART_END;
	}

	state->startc = state->cursor + 1;
	ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
	// fall thru

    case PSYC_PART_CONTENT:
	// In case of an incomplete binary variable resume parsing it.
	if (state->value_parsed < state->valuelen) {
	    ret = psyc_parse_binary_value(state, value, &(state->valuelen),
					  &(state->value_parsed));
	    state->content_parsed += value->length;

	    if (ret == PARSE_INCOMPLETE)
		return PSYC_PARSE_ENTITY_CONT;

	    return PSYC_PARSE_ENTITY_END;
	}

	pos = state->cursor;

	if (state->content_parsed > 0) {
	    if (state->buffer.data[state->cursor] != '\n')
		return PSYC_PARSE_ERROR_MOD_NL;
	    ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
	}
	// Each line of the header starts with a glyph,
	// i.e. :_name, -_name +_name etc.
	// So just test if the first char is a glyph.
	// In the body, the same applies, only that the
	// method does not start with a glyph.
	if (psyc_is_glyph(state->buffer.data[state->cursor])) {
	    if (state->content_parsed == 0) {
		ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
		if (state->buffer.data[state->cursor] == '\n') {
		    *oper = *(state->buffer.data + state->cursor - 1);
		    switch (*oper) {
		    case PSYC_STATE_RESYNC:
			state->content_parsed++;
			return PSYC_PARSE_STATE_RESYNC;
		    case PSYC_STATE_RESET:
			state->content_parsed++;
			return PSYC_PARSE_STATE_RESET;
		    default:
			return PSYC_PARSE_ERROR_MOD_NAME;
		    }
		}
		state->cursor--;
	    }

	    ret = psyc_parse_modifier(state, oper, name, value);
	    state->content_parsed += state->cursor - pos;

	    if (ret == PARSE_INCOMPLETE)
		return PSYC_PARSE_ENTITY_START;
	    else if (ret == PARSE_SUCCESS)
		return PSYC_PARSE_ENTITY;

	    return ret;
	} else {
	    state->content_parsed += state->cursor - pos;
	    state->startc = state->cursor;
	    state->part = PSYC_PART_METHOD;
	    // fall thru
	}

    case PSYC_PART_METHOD:
	pos = state->cursor;
	ret = parse_keyword(state, name);

	if (ret == PARSE_INSUFFICIENT)
	    return ret;
	else if (ret == PARSE_SUCCESS) {
	    // The method ends with a \n then the data follows.
	    if (state->buffer.data[state->cursor] != '\n')
		return PSYC_PARSE_ERROR_METHOD;

	    state->valuelen_found = 0;
	    state->value_parsed = 0;
	    state->valuelen = 0;

	    if (state->contentlen_found) {
		// len found, set start position to the beginning of data.
		state->cursor++;
		state->startc = state->cursor;
		state->content_parsed += state->cursor - pos;
		state->part = PSYC_PART_DATA;
	    } else { // Otherwise keep it at the beginning of method.
		ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
	    }
	} else { // No method, which means the packet should end now.
	    state->part = PSYC_PART_END;
	    state->startc = state->cursor;
	    goto PSYC_PART_END;
	}
	// fall thru

    case PSYC_PART_DATA:
    PSYC_PART_DATA:
	value->data = state->buffer.data + state->cursor;
	value->length = 0;

	if (state->contentlen_found) { // We know the length of the packet.
	    if (!state->valuelen_found) { // start of data
		state->valuelen_found = 1;
		state->valuelen = state->contentlen - state->content_parsed;
		if (state->valuelen && !(state->flags & PSYC_PARSE_ROUTING_ONLY))
		    state->valuelen--; // \n at the end is not part of data
	    }
	    if (state->value_parsed < state->valuelen) {
		ret = psyc_parse_binary_value(state, value, &(state->valuelen),
					      &(state->value_parsed));
		state->content_parsed += value->length;

		if (ret == PARSE_INCOMPLETE)
		    return state->value_parsed == value->length
			? PSYC_PARSE_BODY_START : PSYC_PARSE_BODY_CONT;
	    }

	    state->part = PSYC_PART_END;
	    return state->valuelen == value->length ?
		PSYC_PARSE_BODY : PSYC_PARSE_BODY_END;
	} else { // Search for the terminator.
	    size_t datac = state->cursor; // start of data
	    if (state->flags & PSYC_PARSE_ROUTING_ONLY) // in routing-only mode restart
		state->startc = datac;			// from the start of data

	    while (1) {
		uint8_t nl = state->buffer.data[state->cursor] == '\n';
		// check for |\n if we're at the start of data or we have found a \n
		if (state->cursor == datac || nl) {
		    // incremented cursor inside length?
		    if (state->cursor + 1 + nl >= state->buffer.length) {
			state->cursor = state->startc;
			return PSYC_PARSE_INSUFFICIENT;
		    }

		    if (state->buffer.data[state->cursor + nl] == '|'
			&& state->buffer.data[state->cursor + 1 + nl] == '\n') {
			// packet ends here
			if (state->flags & PSYC_PARSE_ROUTING_ONLY)
			    value->length++;

			state->content_parsed += state->cursor - pos;
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
	// if data was not empty next is the \n at the end of data
	if (state->contentlen_found && state->valuelen_found
	    && state->valuelen && !(state->flags & PSYC_PARSE_ROUTING_ONLY)) {
	    state->valuelen = 0;
	    state->valuelen_found = 0;

	    if (state->buffer.data[state->cursor] != '\n')
		return PSYC_PARSE_ERROR_END;

	    state->content_parsed++;
	    state->cursor++;
	}
	// End of packet, at this point we have already passed a \n
	// and the cursor should point to |
	if (state->cursor + 1 >= state->buffer.length)
	    return PSYC_PARSE_INSUFFICIENT;

	if (state->buffer.data[state->cursor] == '|'
	    && state->buffer.data[state->cursor + 1] == '\n') {
	    // Packet ends here.
	    state->cursor += 2;
	    state->part = PSYC_PART_RESET;
	    return PSYC_PARSE_COMPLETE;
	} else { // Packet should've ended here, return error.
	    state->part = PSYC_PART_RESET;
	    return PSYC_PARSE_ERROR_END;
	}
    }
    return PSYC_PARSE_ERROR; // should not be reached
}

/** List parser. */
#ifdef __INLINE_PSYC_PARSE
static inline
#endif
PsycParseListRC
psyc_parse_list (PsycParseListState *state, PsycString *elem)
{
    if (state->cursor >= state->buffer.length)
	return PSYC_PARSE_LIST_INCOMPLETE;

    state->startc = state->cursor;

    if (!state->type) {	// If type is not set we're at the start.
	// First character is either | for text lists, or a number for binary lists
	if (state->buffer.data[state->cursor] == '|') {
	    state->type = PSYC_LIST_TEXT;
	    state->cursor++;
	} else if (psyc_is_numeric(state->buffer.data[state->cursor]))
	    state->type = PSYC_LIST_BINARY;
	else
	    return PSYC_PARSE_LIST_ERROR_TYPE;
    }

    if (state->type == PSYC_LIST_TEXT) {
	elem->data = state->buffer.data + state->cursor;
	elem->length = 0;

	if (state->cursor >= state->buffer.length)
	    return PSYC_PARSE_LIST_END;

	if (state->term_set) {
	    while (state->buffer.data[state->cursor] != '|') {
		elem->length++;
		if (state->buffer.data[state->cursor] == state->term)
		    return PSYC_PARSE_LIST_END;
		if (++(state->cursor) >= state->buffer.length)
		    return PSYC_PARSE_LIST_END;
	    }
	} else {
	    while (state->buffer.data[state->cursor] != '|') {
		elem->length++;
		if (++(state->cursor) >= state->buffer.length)
		    return PSYC_PARSE_LIST_END;
	    }
	}
	state->cursor++;
	return PSYC_PARSE_LIST_ELEM;
    } else { // binary list
	if (!(state->elem_parsed < state->elemlen)) {
	    // Element starts with a number.
	    if (psyc_is_numeric(state->buffer.data[state->cursor])) {
		do {
		    state->elemlen =
			10 * state->elemlen +
			state->buffer.data[state->cursor] - '0';
		    ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_LIST_INCOMPLETE);
		} while (psyc_is_numeric(state->buffer.data[state->cursor]));
	    } else
		return PSYC_PARSE_LIST_ERROR_LEN;

	    if (state->buffer.data[state->cursor] != ' ')
		return PSYC_PARSE_LIST_ERROR_LEN;

	    state->cursor++;
	    elem->data = state->buffer.data + state->cursor;
	    elem->length = 0;
	    state->elem_parsed = 0;
	}
	// Start or resume parsing the binary data
	if (state->elem_parsed < state->elemlen) {
	    if (PARSE_INCOMPLETE == psyc_parse_binary_value((PsycParseState*)state,
							    elem, &state->elemlen,
							    &state->elem_parsed))
		return PSYC_PARSE_LIST_INCOMPLETE;

	    state->elemlen = 0;

	    if (state->cursor >= state->buffer.length)
		return PSYC_PARSE_LIST_END;

	    if (state->buffer.data[state->cursor] != '|')
		return PSYC_PARSE_LIST_ERROR_DELIM;

	    state->cursor++;
	    return PSYC_PARSE_LIST_ELEM;
	}
    }

    return PSYC_PARSE_LIST_ERROR; // should not be reached
}

PsycParseTableRC
psyc_parse_table (PsycParseTableState *state, PsycString *elem)
{
    if (state->cursor >= state->buffer.length)
	return PSYC_PARSE_TABLE_INCOMPLETE;

    state->startc = state->cursor;

    switch (state->part) {
    case PSYC_TABLE_PART_START:
	if (state->buffer.data[state->cursor] != '*') {
	    state->part = PSYC_TABLE_PART_BODY_START;
	    goto PSYC_TABLE_PART_BODY_START;
	} else {
	    state->part = PSYC_TABLE_PART_WIDTH;
	    ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_TABLE_INCOMPLETE);
	}
	// fall thru

    case PSYC_TABLE_PART_WIDTH:
	if (psyc_is_numeric(state->buffer.data[state->cursor])) {
	    do {
		state->width =
		    10 * state->width + state->buffer.data[state->cursor] - '0';
		ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_TABLE_INCOMPLETE);
	    } while (psyc_is_numeric(state->buffer.data[state->cursor]));
	} else
	    return PSYC_PARSE_TABLE_ERROR_WIDTH;

	switch (state->buffer.data[state->cursor]) {
#ifdef PSYC_PARSE_TABLE_HEAD
	case '|':
	    state->part = PSYC_TABLE_PART_HEAD_START;
	    break;
#endif
	case ' ':
	    state->part = PSYC_TABLE_PART_BODY_START;
	    state->cursor++;
	}

	elem->length = state->width;
	return PSYC_TABLE_PART_WIDTH;
#ifdef PSYC_PARSE_TABLE_HEAD
    case PSYC_TABLE_PART_HEAD_START:
	psyc_parse_list_buffer_set(&state->list, state->buffer.data + state->cursor,
				   state->buffer.length - state->cursor);
	psyc_parse_list_term_set(&state->list, ' ');
	state->part = PSYC_TABLE_PART_HEAD;
	// fall thru

    case PSYC_TABLE_PART_HEAD:
	switch (psyc_parse_list(&state->list, elem)) {
	case PSYC_PARSE_LIST_ELEM:
	    if (state->elems == 0) {
		state->elems++;
		return PSYC_PARSE_TABLE_NAME_KEY;
	    } else if (state->elems < state->width) {
		state->elems++;
		return PSYC_PARSE_TABLE_NAME_VALUE;
	    } else // too many elements
		return PSYC_PARSE_TABLE_ERROR_HEAD;

	case PSYC_PARSE_LIST_END:
	    if (state->elems != state->width)
		return PSYC_PARSE_TABLE_ERROR_HEAD;

	    state->part = PSYC_TABLE_PART_BODY_START;
	    state->cursor += state->list.cursor + 1;
	    psyc_parse_list_state_init(&state->list);
	    return state->elems++ == 0
		? PSYC_PARSE_TABLE_NAME_KEY : PSYC_PARSE_TABLE_NAME_VALUE;
	default:
	    return PSYC_PARSE_TABLE_ERROR_HEAD;
	}
#endif
    case PSYC_TABLE_PART_BODY_START:
    PSYC_TABLE_PART_BODY_START:
	psyc_parse_list_buffer_set(&state->list, state->buffer.data + state->cursor,
				   state->buffer.length - state->cursor);
	state->part = PSYC_TABLE_PART_BODY;
	// fall thru

    case PSYC_TABLE_PART_BODY:
	switch (psyc_parse_list(&state->list, elem)) {
	case PSYC_PARSE_LIST_ELEM:
	    return state->elems++ % (state->width + 1) == 0
		? PSYC_PARSE_TABLE_KEY : PSYC_PARSE_TABLE_VALUE;
	case PSYC_PARSE_LIST_END:
	    return state->elems++ % (state->width + 1) == 0
		? PSYC_PARSE_TABLE_KEY_END : PSYC_PARSE_TABLE_VALUE_END;
	default:
	    return PSYC_PARSE_TABLE_ERROR_BODY;
	}
    }

    return PSYC_PARSE_LIST_ERROR; // should not be reached
}
