/*
  This file is part of libpsyc.
  Copyright (C) 2011,2012 Carlo v. Loesch, Gabor X Toth, Mathias L. Baumann,
  and other contributing authors.

  libpsyc is free software: you can redistribute it and/or modify it under the
  terms of the GNU Affero General Public License as published by the Free
  Software Foundation, either version 3 of the License, or (at your option) any
  later version. As a special exception, libpsyc is distributed with additional
  permissions to link libpsyc libraries with non-AGPL works.

  libpsyc is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
  details.

  You should have received a copy of the GNU Affero General Public License and
  the linking exception along with libpsyc in a COPYING file.
*/

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

#define ADVANCE_STARTC_OR_RETURN(ret)					\
    state->startc = state->cursor + 1;					\
    if (++(state->cursor) >= state->buffer.length)			\
	return ret;							\

typedef enum {
    PARSE_ERROR = -1,
    PARSE_SUCCESS = 0,
    PARSE_INSUFFICIENT = 1,
    PARSE_INCOMPLETE = 2,
} ParseRC;

typedef struct {
    PsycString buffer;
    size_t cursor;
    size_t startc;
} ParseState;

extern inline void
psyc_parse_state_init (PsycParseState *state, uint8_t flags);

extern inline void
psyc_parse_buffer_set (PsycParseState *state, const char *buffer, size_t length);

extern inline void
psyc_parse_list_state_init (PsycParseListState *state);

extern inline void
psyc_parse_list_buffer_set (PsycParseListState *state,
			    const char *buffer, size_t length);

extern inline void
psyc_parse_dict_state_init (PsycParseDictState *state);

extern inline void
psyc_parse_dict_buffer_set (PsycParseDictState *state,
			     const char *buffer, size_t length);

extern inline void
psyc_parse_index_state_init (PsycParseIndexState *state);

extern inline void
psyc_parse_index_buffer_set (PsycParseIndexState *state,
			     const char *buffer, size_t length);

extern inline void
psyc_parse_update_state_init (PsycParseUpdateState *state);

extern inline void
psyc_parse_update_buffer_set (PsycParseUpdateState *state,
			     const char *buffer, size_t length);

extern inline size_t
psyc_parse_content_length (PsycParseState *state);

extern inline PsycBool
psyc_parse_content_length_found (PsycParseState *state);

extern inline size_t
psyc_parse_value_length (PsycParseState *state);

extern inline PsycBool
psyc_parse_value_length_found (PsycParseState *state);

extern inline size_t
psyc_parse_cursor (PsycParseState *state);

extern inline size_t
psyc_parse_buffer_length (PsycParseState *state);

extern inline size_t
psyc_parse_remaining_length (PsycParseState *state);

extern inline const char *
psyc_parse_remaining_buffer (PsycParseState *state);


/**
 * Parse variable name or method name.
 *
 * It should contain one or more keyword characters.
 *
 * @return PARSE_ERROR or PARSE_SUCCESS
 */
static inline ParseRC
parse_keyword (ParseState *state, PsycString *name)
{
    name->data = state->buffer.data + state->cursor;
    name->length = 0;

    while (psyc_is_kw_char(state->buffer.data[state->cursor])) {
	name->length++;	// was a valid char, increase length
	ADVANCE_CURSOR_OR_RETURN(PARSE_INSUFFICIENT);
    }

    return name->length > 0 ? PARSE_SUCCESS : PARSE_ERROR;
}

/**
 * Parse length.
 *
 * @return PARSE_SUCCESS, PARSE_ERROR or PARSE_INSUFFICIENT
 */
static inline ParseRC
parse_length (ParseState *state, size_t *len)
{
    ParseRC ret = PARSE_ERROR;
    *len = 0;

    if (psyc_is_numeric(state->buffer.data[state->cursor])) {
	ret = PARSE_SUCCESS;
	do {
	    *len = 10 * *len + state->buffer.data[state->cursor] - '0';
	    ADVANCE_CURSOR_OR_RETURN(PARSE_INSUFFICIENT);
	} while (psyc_is_numeric(state->buffer.data[state->cursor]));
    }

    return ret;
}

/**
 * Parse binary data.
 *
 * @param state Parser state.
 * @param value Start & length of parsed data is saved here.
 * @param length Expected length of the data.
 * @param parsed Number of bytes parsed so far.
 *
 * @return PARSE_SUCCESS or PARSE_INCOMPLETE
 */
static inline ParseRC
parse_binary (ParseState *state, size_t length, PsycString *value, size_t *parsed)
{
    size_t remaining = length - *parsed;
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
    ASSERT(*parsed == length);

    return PARSE_SUCCESS;
}

/**
 * Parse data until a given character is found.
 *
 * @param state  Parser state.
 * @param value  Start & length of parsed data is saved here.
 * @param end    Parse until this character is found.
 * @param parsed Number of bytes parsed so far.
 *
 * @return PARSE_SUCCESS or PARSE_INSUFFICIENT
 */
static inline ParseRC
parse_until (ParseState *state, const char end, PsycString *value)
{
    value->data = state->buffer.data + state->cursor;

    while (state->buffer.data[state->cursor] != end) {
	value->length++;
	ADVANCE_CURSOR_OR_RETURN(PARSE_INSUFFICIENT);
    }

    return PARSE_SUCCESS;
}

/**
 * Parse simple or binary variable.
 * @return PARSE_ERROR or PARSE_SUCCESS
 */
#ifdef __INLINE_PSYC_PARSE
extern inline
#endif
ParseRC
psyc_parse_modifier (PsycParseState *state, char *oper,
		     PsycString *name, PsycString *value)
{
    *oper = *(state->buffer.data + state->cursor);
    ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);

    ParseRC ret = parse_keyword((ParseState*)state, name);
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

	ret = parse_binary((ParseState*)state, state->valuelen, value, &state->value_parsed);
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
	if (psyc_is_oper(state->buffer.data[state->cursor])) {
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
	    ret = parse_binary((ParseState*)state, state->valuelen, value,
			       &state->value_parsed);
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
	if (psyc_is_oper(state->buffer.data[state->cursor])) {
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
	ret = parse_keyword((ParseState*)state, name);

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
		ret = parse_binary((ParseState*)state, state->valuelen, value,
				   &state->value_parsed);
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

/**
 * Parse list.
 *
 * list		= [ default-type ] *list-elem
 * list-elem	= "|" ( type [ SP list-value ] / [ length ] [ ":" type ] [ SP *OCTET ] )
 * list-value	= %x00-7B / %x7D-FF	; any byte except "|"
 */
#ifdef __INLINE_PSYC_PARSE
extern inline
#endif
PsycParseListRC
psyc_parse_list (PsycParseListState *state, PsycString *type, PsycString *elem)
{
    ParseRC ret;

    if (state->cursor >= state->buffer.length)
	return PSYC_PARSE_LIST_END;

    state->startc = state->cursor;

    switch (state->part) {
    case PSYC_LIST_PART_START:
	type->length = elem->length = 0;
	type->data = elem->data = NULL;

	state->part = PSYC_LIST_PART_TYPE;
	// fall thru

    case PSYC_LIST_PART_TYPE:
	switch (parse_keyword((ParseState*)state, type)) {
	case PARSE_SUCCESS: // end of keyword
	    state->part = PSYC_LIST_PART_ELEM_START;
	    return PSYC_PARSE_LIST_TYPE;
	case PARSE_INSUFFICIENT: // end of buffer
	    return PSYC_PARSE_LIST_END;
	case PARSE_ERROR: // no keyword
	    state->part = PSYC_LIST_PART_ELEM_START;
	    break;
	default: // should not be reached
	    return PSYC_PARSE_LIST_ERROR;
	}
	// fall thru

    case PSYC_LIST_PART_ELEM_START:
	if (state->buffer.data[state->cursor] != '|')
	    return PSYC_PARSE_LIST_ERROR_ELEM_START;

	type->length = elem->length = 0;
	type->data = elem->data = NULL;

	state->elem_parsed = 0;
	state->elemlen_found = 0;

	state->part = PSYC_LIST_PART_ELEM_LENGTH;
	ADVANCE_STARTC_OR_RETURN(PSYC_PARSE_LIST_ELEM_LAST);
	// fall thru

    case PSYC_LIST_PART_ELEM_TYPE:
	if (state->buffer.data[state->cursor] == '=') {
	    ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);

	    switch (parse_keyword((ParseState*)state, type)) {
	    case PARSE_SUCCESS:
		switch (state->buffer.data[state->cursor]) {
		case ':':
		    state->part = PSYC_LIST_PART_ELEM_LENGTH;
		    ADVANCE_STARTC_OR_RETURN(PSYC_PARSE_LIST_ELEM_LAST);
		    break;
		case ' ':
		    state->part = PSYC_LIST_PART_ELEM;
		    ADVANCE_STARTC_OR_RETURN(PSYC_PARSE_LIST_ELEM_LAST);
		    goto PSYC_LIST_PART_ELEM;
		case '|':
		    state->part = PSYC_LIST_PART_ELEM_START;
		    return PSYC_PARSE_LIST_ELEM;
		    break;
		default:
		    return PSYC_PARSE_LIST_ERROR_ELEM_TYPE;
		}
		break;
	    case PARSE_INSUFFICIENT: // end of buffer
		return PSYC_PARSE_LIST_ELEM_LAST;
	    case PARSE_ERROR:
		return PSYC_PARSE_LIST_ERROR_ELEM_TYPE;
	    default: // should not be reached
		return PSYC_PARSE_LIST_ERROR;
	    }
	}
	// fall thru

    case PSYC_LIST_PART_ELEM_LENGTH:
	switch (parse_length((ParseState*)state, &state->elemlen)) {
	case PARSE_SUCCESS: // length is complete
	    state->elemlen_found = 1;
	    state->elem_parsed = 0;
	    elem->length = state->elemlen;
	    elem->data = NULL;
	    break;
	case PARSE_INSUFFICIENT: // length is incomplete
	    return PSYC_PARSE_LIST_INSUFFICIENT;
	case PARSE_ERROR: // no length
	    break;
	default: // should not be reached
	    return PSYC_PARSE_LIST_ERROR;
	}

	switch (state->buffer.data[state->cursor]) {
	case ' ':
	    state->part = PSYC_LIST_PART_ELEM;
	    ADVANCE_STARTC_OR_RETURN(PSYC_PARSE_LIST_ELEM_LAST);
	    break;
	case '|':
	    state->part = PSYC_LIST_PART_ELEM_START;
	    return PSYC_PARSE_LIST_ELEM;
	default:
	    return PSYC_PARSE_LIST_ERROR_ELEM_LENGTH;
	}
	// fall thru

    case PSYC_LIST_PART_ELEM:
    PSYC_LIST_PART_ELEM:
	if (state->elemlen_found) {
	    switch (parse_binary((ParseState*)state, state->elemlen, elem,
				 &state->elem_parsed)) {
	    case PARSE_SUCCESS:
	        state->part = PSYC_LIST_PART_ELEM_START;
		if (elem->length == state->elem_parsed)
		    ret = PSYC_PARSE_LIST_ELEM;
		else
		    ret = PSYC_PARSE_LIST_ELEM_END;
		break;
	    case PARSE_INCOMPLETE:
                state->part = PSYC_LIST_PART_ELEM;
		if (elem->length == state->elem_parsed)
		    ret = PSYC_PARSE_LIST_ELEM_START;
		else
		    ret = PSYC_PARSE_LIST_ELEM_CONT;
		break;
	    default: // should not be reached
		return PSYC_PARSE_LIST_ERROR;
	    }
	} else {
	    switch (parse_until((ParseState*)state, '|', elem)) {
	    case PARSE_SUCCESS:
	        state->part = PSYC_LIST_PART_ELEM_START;
		ret = PSYC_PARSE_LIST_ELEM;
		break;
	    case PARSE_INSUFFICIENT:
		return PSYC_PARSE_LIST_ELEM_LAST;
	    default: // should not be reached
		return PSYC_PARSE_LIST_ERROR;
	    }
	}

	state->startc = state->cursor;
	return ret;
    }

    return PSYC_PARSE_LIST_ERROR; // should not be reached
}

/**
 * Parse dictionary.
 *
 * dict		= [ type ] *dict-item
 * dict-item	= "{" ( dict-key / length SP OCTET) "}"
 *                ( type [ SP dict-value ] / [ length ] [ ":" type ] [ SP *OCTET ] )
 * dict-key	= %x00-7C / %x7E-FF	; any byte except "{"
 * dict-value	= %x00-7A / %x7C-FF	; any byte except "}"
 */
#ifdef __INLINE_PSYC_PARSE
extern inline
#endif
PsycParseDictRC
psyc_parse_dict (PsycParseDictState *state, PsycString *type, PsycString *elem)
{
    ParseRC ret;

    if (state->cursor >= state->buffer.length)
	return PSYC_PARSE_DICT_END;

    state->startc = state->cursor;

    switch (state->part) {
    case PSYC_DICT_PART_START:
	type->length = elem->length = 0;
	type->data = elem->data = NULL;

	state->part = PSYC_DICT_PART_TYPE;
	// fall thru

    case PSYC_DICT_PART_TYPE:
	switch (parse_keyword((ParseState*)state, type)) {
	case PARSE_SUCCESS: // end of keyword
	    state->part = PSYC_DICT_PART_KEY_START;
	    return PSYC_PARSE_DICT_TYPE;
	case PARSE_INSUFFICIENT: // end of buffer
	    return PSYC_PARSE_DICT_END;
	case PARSE_ERROR: // no keyword
	    state->part = PSYC_DICT_PART_KEY_START;
	    break;
	default: // should not be reached
	    return PSYC_PARSE_DICT_ERROR;
	}
	// fall thru

    case PSYC_DICT_PART_KEY_START:
	if (state->buffer.data[state->cursor] != '{')
	    return PSYC_PARSE_DICT_ERROR_KEY_START;

	type->length = elem->length = 0;
	type->data = elem->data = NULL;

	state->elem_parsed = 0;
	state->elemlen_found = 0;

	state->part = PSYC_DICT_PART_KEY_LENGTH;
	ADVANCE_STARTC_OR_RETURN(PSYC_PARSE_DICT_INSUFFICIENT);
	// fall thru

    case PSYC_DICT_PART_KEY_LENGTH:
	switch (parse_length((ParseState*)state, &state->elemlen)) {
	case PARSE_SUCCESS: // length is complete
	    state->elemlen_found = 1;
	    state->elem_parsed = 0;
	    elem->length = state->elemlen;
	    elem->data = NULL;

	    if (state->buffer.data[state->cursor] != ' ')
		return PSYC_PARSE_DICT_ERROR_KEY_LENGTH;

	    state->part = PSYC_DICT_PART_KEY;
	    ADVANCE_STARTC_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
	    break;
	case PARSE_INSUFFICIENT: // length is incomplete
	    return PSYC_PARSE_DICT_INSUFFICIENT;
	case PARSE_ERROR: // no length
	    state->part = PSYC_DICT_PART_KEY;
	    break;
	default: // should not be reached
	    return PSYC_PARSE_DICT_ERROR;
	}
	// fall thru

    case PSYC_DICT_PART_KEY:
	if (state->elemlen_found) {
	    switch (parse_binary((ParseState*)state, state->elemlen, elem,
				 &state->elem_parsed)) {
	    case PARSE_SUCCESS:
		if (elem->length == state->elem_parsed)
		    ret = PSYC_PARSE_DICT_KEY;
		else
		    ret = PSYC_PARSE_DICT_KEY_END;
		break;
	    case PARSE_INCOMPLETE:
		if (elem->length == state->elem_parsed)
		    ret = PSYC_PARSE_DICT_KEY_START;
		else
		    ret = PSYC_PARSE_DICT_KEY_CONT;
		break;
	    default: // should not be reached
		return PSYC_PARSE_DICT_ERROR;
	    }
	} else {
	    switch (parse_until((ParseState*)state, '}', elem)) {
	    case PARSE_SUCCESS:
		ret = PSYC_PARSE_DICT_KEY;
		break;
	    case PARSE_INSUFFICIENT:
		return PSYC_PARSE_DICT_INSUFFICIENT;
	    default: // should not be reached
		return PSYC_PARSE_DICT_ERROR;
	    }
	}

	state->part = PSYC_DICT_PART_VALUE_START;
	state->startc = state->cursor;
	return ret;

    case PSYC_DICT_PART_VALUE_START:
	switch (state->buffer.data[state->cursor] != '}')
	    return PSYC_PARSE_DICT_ERROR_VALUE_START;

	type->length = elem->length = 0;
	type->data = elem->data = NULL;

	state->elem_parsed = 0;
	state->elemlen_found = 0;

	state->part = PSYC_DICT_PART_VALUE_TYPE;
	ADVANCE_STARTC_OR_RETURN(PSYC_PARSE_DICT_VALUE_LAST);
	// fall thru

    case PSYC_DICT_PART_VALUE_TYPE:
	if (state->buffer.data[state->cursor] == '=') {
	    ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_INSUFFICIENT);

	    switch (parse_keyword((ParseState*)state, type)) {
	    case PARSE_SUCCESS:
		switch (state->buffer.data[state->cursor]) {
		case ':':
		    state->part = PSYC_DICT_PART_VALUE_LENGTH;
		    ADVANCE_STARTC_OR_RETURN(PSYC_PARSE_DICT_VALUE_LAST);
		    break;
		case ' ':
		    state->part = PSYC_DICT_PART_VALUE;
		    ADVANCE_STARTC_OR_RETURN(PSYC_PARSE_DICT_VALUE_LAST);
		    goto PSYC_DICT_PART_VALUE;
		case '{':
		    state->part = PSYC_DICT_PART_KEY_START;
		    return PSYC_PARSE_DICT_VALUE;
		    break;
		default:
		    return PSYC_PARSE_DICT_ERROR_VALUE_TYPE;
		}
		break;
	    case PARSE_INSUFFICIENT: // end of buffer
		return PSYC_PARSE_DICT_VALUE_LAST;
	    case PARSE_ERROR:
		return PSYC_PARSE_DICT_ERROR_VALUE_TYPE;
	    default: // should not be reached
		return PSYC_PARSE_DICT_ERROR;
	    }
	}
	// fall thru

    case PSYC_DICT_PART_VALUE_LENGTH:
	switch (parse_length((ParseState*)state, &state->elemlen)) {
	case PARSE_SUCCESS: // length is complete
	    state->elemlen_found = 1;
	    state->elem_parsed = 0;
	    elem->length = state->elemlen;
	    elem->data = NULL;
	    break;
	case PARSE_INSUFFICIENT: // length is incomplete
	    return PSYC_PARSE_DICT_INSUFFICIENT;
	case PARSE_ERROR: // no length
	    break;
	default: // should not be reached
	    return PSYC_PARSE_DICT_ERROR;
	}

	switch (state->buffer.data[state->cursor]) {
	case ' ':
	    state->part = PSYC_DICT_PART_VALUE;
	    ADVANCE_STARTC_OR_RETURN(PSYC_PARSE_DICT_VALUE_LAST);
	    break;
	case '{':
	    state->part = PSYC_DICT_PART_KEY_START;
	    return PSYC_PARSE_DICT_VALUE;
	default:
	    return PSYC_PARSE_DICT_ERROR_VALUE_LENGTH;
	}
	// fall thru

    case PSYC_DICT_PART_VALUE:
    PSYC_DICT_PART_VALUE:
	if (state->elemlen_found) {
	    switch (parse_binary((ParseState*)state, state->elemlen, elem,
				 &state->elem_parsed)) {
	    case PARSE_SUCCESS:
	    state->part = PSYC_DICT_PART_KEY_START;
		if (elem->length == state->elem_parsed)
		    ret = PSYC_PARSE_DICT_VALUE;
		else
		    ret = PSYC_PARSE_DICT_VALUE_END;
		break;
	    case PARSE_INCOMPLETE:
		if (elem->length == state->elem_parsed)
		    ret = PSYC_PARSE_DICT_VALUE_START;
		else
		    ret = PSYC_PARSE_DICT_VALUE_CONT;
		break;
	    default: // should not be reached
		return PSYC_PARSE_DICT_ERROR;
	    }
	} else {
	    switch (parse_until((ParseState*)state, '{', elem)) {
	    case PARSE_SUCCESS:
	    state->part = PSYC_DICT_PART_KEY_START;
		ret = PSYC_PARSE_DICT_VALUE;
		break;
	    case PARSE_INSUFFICIENT:
		return PSYC_PARSE_DICT_VALUE_LAST;
	    default: // should not be reached
		return PSYC_PARSE_DICT_ERROR;
	    }
	}

	//state->part = PSYC_DICT_PART_KEY_START;
	return ret;
    }

    return PSYC_PARSE_DICT_ERROR; // should not be reached
}

#ifdef __INLINE_PSYC_PARSE
extern inline
#endif
PsycParseIndexRC
psyc_parse_index (PsycParseIndexState *state, PsycString *idx)
{
    ParseRC ret;

    if (state->cursor >= state->buffer.length)
	return PSYC_PARSE_INDEX_END;

    state->startc = state->cursor;

    switch (state->part) {
    case PSYC_INDEX_PART_START:
    case PSYC_INDEX_PART_TYPE:
	idx->length = 0;
	idx->data = NULL;

	switch (state->buffer.data[state->cursor]) {
	case '#':
	    state->part = PSYC_INDEX_PART_LIST;
	    ADVANCE_STARTC_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
	    goto PSYC_INDEX_PART_LIST;
	case '.':
	    state->part = PSYC_INDEX_PART_DICT;
	    ADVANCE_STARTC_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
	    goto PSYC_INDEX_PART_STRUCT;
	case '{':
	    state->part = PSYC_INDEX_PART_DICT;
	    ADVANCE_STARTC_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
	    goto PSYC_INDEX_PART_DICT_LENGTH;
	default:
	    return PSYC_PARSE_INDEX_ERROR_TYPE;
	}

    case PSYC_INDEX_PART_LIST:
    PSYC_INDEX_PART_LIST:
	switch (parse_length((ParseState*)state, &idx->length)) {
	case PARSE_SUCCESS: // list index is complete
	    state->part = PSYC_INDEX_PART_TYPE;
	    return PSYC_PARSE_INDEX_LIST;
	case PARSE_INSUFFICIENT: // list index at the end of buffer
	    return PSYC_PARSE_INDEX_LIST_LAST;
	case PARSE_ERROR: // no index
	    return PSYC_PARSE_INDEX_ERROR_LIST;
	default: // should not be reached
	    return PSYC_PARSE_INDEX_ERROR;
	}

    case PSYC_INDEX_PART_STRUCT:
    PSYC_INDEX_PART_STRUCT:
	switch (parse_keyword((ParseState*)state, idx)) {
	case PARSE_SUCCESS: // end of keyword
	    state->part = PSYC_INDEX_PART_TYPE;
	    return PSYC_PARSE_INDEX_STRUCT;
	case PARSE_INSUFFICIENT: // end of buffer
	    return PSYC_PARSE_INDEX_STRUCT_LAST;
	case PARSE_ERROR: // no keyword
	    return PSYC_PARSE_INDEX_ERROR_STRUCT;
	default: // should not be reached
	    return PSYC_PARSE_INDEX_ERROR;
	}

    case PSYC_INDEX_PART_DICT_LENGTH:
    PSYC_INDEX_PART_DICT_LENGTH:
	switch (parse_length((ParseState*)state, &state->elemlen)) {
	case PARSE_SUCCESS: // length is complete
	    state->elemlen_found = 1;
	    state->elem_parsed = 0;
	    idx->length = state->elemlen;
	    idx->data = NULL;

	    if (state->buffer.data[state->cursor] != ' ')
		return PSYC_PARSE_INDEX_ERROR_DICT_LENGTH;

	    state->part = PSYC_INDEX_PART_DICT;
	    ADVANCE_STARTC_OR_RETURN(PSYC_PARSE_INSUFFICIENT);
	    break;
	case PARSE_INSUFFICIENT: // length is incomplete
	    return PSYC_PARSE_DICT_INSUFFICIENT;
	case PARSE_ERROR: // no length
	    state->part = PSYC_INDEX_PART_DICT;
	    break;
	default: // should not be reached
	    return PSYC_PARSE_INDEX_ERROR;
	}
	// fall thru

    case PSYC_INDEX_PART_DICT:
	if (state->elemlen_found) {
	    switch (parse_binary((ParseState*)state, state->elemlen, idx,
				 &state->elem_parsed)) {
	    case PARSE_SUCCESS:
		if (idx->length == state->elem_parsed)
		    ret = PSYC_PARSE_INDEX_DICT;
		else
		    ret = PSYC_PARSE_INDEX_DICT_END;
		break;
	    case PARSE_INCOMPLETE:
		if (idx->length == state->elem_parsed)
		    ret = PSYC_PARSE_INDEX_DICT_START;
		else
		    ret = PSYC_PARSE_INDEX_DICT_CONT;
		break;
	    default: // should not be reached
		return PSYC_PARSE_INDEX_ERROR_DICT;
	    }
	} else {
	    switch (parse_until((ParseState*)state, '}', idx)) {
	    case PARSE_SUCCESS:
		ret = PSYC_PARSE_INDEX_DICT;
		break;
	    case PARSE_INSUFFICIENT:
		return PSYC_PARSE_INDEX_INSUFFICIENT;
	    default: // should not be reached
		return PSYC_PARSE_INDEX_ERROR_DICT;
	    }
	}

	state->part = PSYC_INDEX_PART_TYPE;
	state->cursor++;
	return ret;
    }

    return PSYC_PARSE_INDEX_ERROR; // should not be reached
}

#ifdef __INLINE_PSYC_PARSE
extern inline
#endif
PsycParseUpdateRC
psyc_parse_update (PsycParseUpdateState *state, char *oper, PsycString *value)
{
    PsycParseIndexRC ret;

    if (state->cursor >= state->buffer.length)
	return PSYC_PARSE_UPDATE_END;

    state->startc = state->cursor;

    switch (state->part) {
    case PSYC_UPDATE_PART_START:
	value->length = 0;
	value->data = NULL;
	// fall thru

    case PSYC_INDEX_PART_TYPE:
    case PSYC_INDEX_PART_LIST:
    case PSYC_INDEX_PART_STRUCT:
    case PSYC_INDEX_PART_DICT_LENGTH:
    case PSYC_INDEX_PART_DICT:
	ret = psyc_parse_index((PsycParseIndexState*)state, value);

	switch (ret) {
	case PSYC_PARSE_INDEX_INSUFFICIENT:
	case PSYC_PARSE_INDEX_LIST_LAST:
	case PSYC_PARSE_INDEX_STRUCT_LAST:
	case PSYC_PARSE_INDEX_END:
	    return PSYC_PARSE_UPDATE_INSUFFICIENT;
	case PSYC_PARSE_INDEX_ERROR_TYPE:
	    if (state->buffer.data[state->cursor] != ' ')
		return ret;
	    state->part = PSYC_PARSE_UPDATE_TYPE;
	    value->length = 0;
	    value->data = NULL;
	    ADVANCE_STARTC_OR_RETURN(PSYC_PARSE_UPDATE_INSUFFICIENT);
	    break;
	default:
	    return ret;
	}
    case PSYC_UPDATE_PART_TYPE:
	if (!psyc_is_oper(state->buffer.data[state->cursor]))
	    return PSYC_PARSE_UPDATE_ERROR_OPER;

	*oper = state->buffer.data[state->cursor];
	ADVANCE_CURSOR_OR_RETURN(PSYC_PARSE_UPDATE_END);

	switch (parse_keyword((ParseState*)state, value)) {
	case PARSE_SUCCESS: // end of keyword
	case PARSE_ERROR: // no keyword
	    switch (state->buffer.data[state->cursor]) {
	    case ':':
		state->part = PSYC_UPDATE_PART_LENGTH;
		break;
	    case ' ':
		state->part = PSYC_UPDATE_PART_VALUE;
		break;
	    default:
		return PSYC_PARSE_UPDATE_ERROR_TYPE;
	    }

	    state->cursor++;
	    return PSYC_PARSE_UPDATE_TYPE;
	    break;
	case PARSE_INSUFFICIENT: // end of buffer
	    return PSYC_PARSE_UPDATE_TYPE_END;
	default: // should not be reached
	    return PSYC_PARSE_UPDATE_ERROR;
	}
	break;

    case PSYC_UPDATE_PART_LENGTH:
	switch (parse_length((ParseState*)state, &state->elemlen)) {
	case PARSE_SUCCESS: // length is complete
	    state->elemlen_found = 1;
	    state->elem_parsed = 0;
	    value->length = state->elemlen;
	    value->data = NULL;

	    if (state->buffer.data[state->cursor] != ' ')
		return PSYC_PARSE_UPDATE_ERROR_LENGTH;

	    state->part = PSYC_UPDATE_PART_VALUE;
	    if (value->length == 0)
		return PSYC_PARSE_UPDATE_END;
	    ADVANCE_STARTC_OR_RETURN(PSYC_PARSE_UPDATE_INSUFFICIENT);
	    break;
	case PARSE_INSUFFICIENT: // length is incomplete
	    if (value->length == 0)
		return PSYC_PARSE_UPDATE_END;
	    return PSYC_PARSE_UPDATE_INSUFFICIENT;
	case PARSE_ERROR: // no length after :
	    return PSYC_PARSE_UPDATE_ERROR_LENGTH;
	default: // should not be reached
	    return PSYC_PARSE_UPDATE_ERROR;
	}
	// fall thru

    case PSYC_UPDATE_PART_VALUE:
	if (state->elemlen_found) {
	    switch (parse_binary((ParseState*)state, state->elemlen, value,
				 &state->elem_parsed)) {
	    case PARSE_SUCCESS:
		if (value->length == state->elem_parsed)
		    ret = PSYC_PARSE_UPDATE_VALUE;
		else
		    ret = PSYC_PARSE_UPDATE_VALUE_END;
		break;
	    case PARSE_INCOMPLETE:
		if (value->length == state->elem_parsed)
		    ret = PSYC_PARSE_UPDATE_VALUE_START;
		else
		    ret = PSYC_PARSE_UPDATE_VALUE_CONT;
		break;
	    default: // should not be reached
		return PSYC_PARSE_UPDATE_ERROR_VALUE;
	    }
	} else {
	    value->data = state->buffer.data + state->cursor;
	    value->length = state->buffer.length - state->cursor;
	    ret = PSYC_PARSE_UPDATE_VALUE;
	}

	state->part = PSYC_INDEX_PART_TYPE;
	state->cursor++;
	return ret;
    }

    return PSYC_PARSE_INDEX_ERROR; // should not be reached
}

extern inline size_t
psyc_parse_int (const char *value, size_t len, int64_t *n);

extern inline size_t
psyc_parse_uint (const char *value, size_t len, uint64_t *n);

extern inline size_t
psyc_parse_list_index (const char *value, size_t len, int64_t *n);

extern inline PsycBool
psyc_is_oper (char g);

extern inline char
psyc_is_numeric (char c);

extern inline char
psyc_is_alpha (char c);

extern inline char
psyc_is_alpha_numeric (char c);

extern inline char
psyc_is_kw_char (char c);

extern inline char
psyc_is_name_char (char c);

extern inline char
psyc_is_host_char (char c);

extern inline size_t
psyc_parse_keyword (const char *data, size_t len);

