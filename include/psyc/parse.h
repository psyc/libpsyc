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

#ifndef PSYC_PARSE_H
#define PSYC_PARSE_H

/**
 * @file psyc/parse.h
 * @brief Interface for PSYC packet parsing.
 *
 * All parsing functions and the definitions they use are defined here.
 */

/**
 * @defgroup parse Parsing Functions
 *
 * This module contains packet and list parsing functions.
 * The parser adheres to the definition of a packet found at
 *
 *   http://about.psyc.eu/Spec:Packet
 *
 * and the according terms are used throughout this documentation and in the
 * return codes. You should be at least
 * vaguely familiar with differences between "body" and "content" as
 * well as "routing variable" and "entity variable".
 *
 *
 * To parse a packet you first have to initialize a state:
 *
 * @code
 * PsycParseState state;
 * psyc_parse_state_init(&state, flags);
 * @endcode
 *
 * With the flags parameter you can fine-tune what
 * part of the packet should be parsed. @see PsycParseFlag
 *
 * Next, you have to tell the parser what it should parse. Assuming the variable
 * raw_data points to our packet and raw_len contains the length, you can pass
 * it to the parser as follows:
 *
 * @code
 * char* raw_data; // points to our (possibly incomplete) packet
 * size_t raw_len; // how many bytes of data
 *
 * // state is our initialized state from before
 * psyc_parse_buffer_set(&state, raw_data, raw_len);
 * @endcode
 *
 * Now the the variables that will save the output of the parser need to be
 * declared:
 *
 * @code
 * PsycString name,  // Name of the variable or method
 *            value; // Value of the variable or body
 * char       oper;  // operator of the variable (if any)
 * @endcode
 *
 * They will be passed to the parsing function which will set them to
 * the according positions and lengths.
 *
 * Now the real parsing begins. The parsing function needs to be called
 * repeatedly with various actions in between, depending on the return values.
 *
 * A simplified example follows, see test/testPsyc.c for actual code that
 * handles incomplete packets as well.
 *
 * @code
 *
 * int ret;
 *
 * do // run the parsing in a loop, each time parsing one line
 * {
 * 	name.length = value.length = oper = 0; // reset the output variables
 *
 * 	ret = psyc_parse(&state, &oper, &name, &value); // call the parsing function
 *
 * 	switch (ret) // look at the return value
 * 	{
 * 		case PSYC_PARSE_ROUTING: // it is a routing variable
 * 		case PSYC_PARSE_ENTITY:  // it is a entity variable
 * 			// Name, value and operator of the variable can now be found in the
 * 			// respective variables:
 * 			printf("Variable: %.*s  Value: %.*s Operator: %c\n",
 * 			        name.length, name.data,
 * 			        value.length, value.data,
 * 			        oper);
 * 			// Note that the .data member still points at your original buffer. If
 * 			// you want to reuse that buffer for the next packet, you better copy it
 * 			// before passing it to the parser or you copy each variable now.
 * 			break;
 * 		case PSYC_PARSE_BODY: // it is the method and the body of the packet.
 * 			printf("Method Name: %.*s  Body: %.*s\n",
 * 			        name.length, name.data,    // name of the method
 * 			        value.length, value.data); // value of the body
 * 			break;
 * 		case PSYC_PARSE_COMPLETE: // parsing of this packet is complete
 * 			// You can simply continue parsing till you get the
 * 			// PSYC_PARSE_INSUFFICIENT code which means the line is incomplete.
 * 			continue;
 * 		default: //
 * 			perror("Error %i happened :(\n", res);
 * 			return res;
 * 	}
 * }
 * while (ret > 0)
 * @endcode
 *
 * This simple example does not consider some more complex cases when you
 * receive incomplete packets but still want to access the data. This code would
 * simply reject incomplete packets as error. A more detailed tutorial for
 * incomplete packets will follow. In the mean time, have look at the return
 * codes in PsycParseRC and their explanations. @see PsycParseRC
 */

/** @{ */ // begin of parser group

#include <stdint.h>
#include <string.h>

#include "../psyc.h"

/** Options for selective parsing. */
typedef enum {
    /// Default Flag. Parse everything.
    PSYC_PARSE_ALL = 0,
    /// Parse only the header
    PSYC_PARSE_ROUTING_ONLY = 1,
    /// Parse only the content.
    /// Parsing starts at the content and the content must be complete.
    PSYC_PARSE_START_AT_CONTENT = 2,
} PsycParseFlag;

/**
 * The return value definitions for the packet parsing function.
 * @see psyc_parse()
 */
typedef enum {
    /// Error, no length is set for a modifier which is longer than PSYC_MODIFIER_SIZE_THRESHOLD.
    PSYC_PARSE_ERROR_MOD_NO_LEN = -10,
    /// Error, no length is set for the content but it is longer than PSYC_CONTENT_SIZE_THRESHOLD.
    PSYC_PARSE_ERROR_NO_LEN = -9,
    /// Error, packet is not ending with a valid delimiter.
    PSYC_PARSE_ERROR_END = -8,
    /// Error, expected NL after the method.
    PSYC_PARSE_ERROR_METHOD = -7,
    /// Error, expected NL after a modifier.
    PSYC_PARSE_ERROR_MOD_NL = -6,
    /// Error, modifier length is not numeric.
    PSYC_PARSE_ERROR_MOD_LEN = -5,
    /// Error, expected TAB before modifier value.
    PSYC_PARSE_ERROR_MOD_TAB = -4,
    /// Error, modifier name is missing.
    PSYC_PARSE_ERROR_MOD_NAME = -3,
    /// Error, expected NL after the content length.
    PSYC_PARSE_ERROR_LENGTH = -2,
    /// Error in packet.
    PSYC_PARSE_ERROR = -1,
    /// Buffer contains insufficient amount of data.
    /// Fill another buffer and concatenate it with the end of the current buffer,
    /// from the cursor position to the end.
    PSYC_PARSE_INSUFFICIENT = 1,
    /// Routing modifier parsing done.
    /// Operator, name & value contains the respective parts.
    PSYC_PARSE_ROUTING = 2,
    /// State sync operation.
    PSYC_PARSE_STATE_RESYNC = 3,
    /// State reset operation.
    PSYC_PARSE_STATE_RESET = 4,
    /// Start of an incomplete entity modifier.
    /// Operator & name are complete, value is incomplete.
    PSYC_PARSE_ENTITY_START = 5,
    /// Continuation of an incomplete entity modifier.
    PSYC_PARSE_ENTITY_CONT = 6,
    /// End of an incomplete entity modifier.
    PSYC_PARSE_ENTITY_END = 7,
    /// Entity modifier parsing done in one go.
    /// Operator, name & value contains the respective parts.
    PSYC_PARSE_ENTITY = 8,
    /// Start of an incomplete body.
    /// Name contains method, value contains part of the body.
    /// Used when packet length is given
    PSYC_PARSE_BODY_START = 9,
    /// Continuation of an incomplete body.
    /// Used when packet length is given
    PSYC_PARSE_BODY_CONT = 10,
    /// End of an incomplete body.
    /// Used when packet length is given
    PSYC_PARSE_BODY_END = 11,
    /// Body parsing done in one go, name contains method, value contains body.
    PSYC_PARSE_BODY = 12,
    /// Start of an incomplete content, value contains part of content.
    /// Used when PSYC_PARSE_ROUTING_ONLY is set.
    PSYC_PARSE_CONTENT_START = 9,
    /// Continuation of an incomplete content.
    /// Used when PSYC_PARSE_ROUTING_ONLY is set.
    PSYC_PARSE_CONTENT_CONT = 10,
    /// End of an incomplete content.
    /// Used when PSYC_PARSE_ROUTING_ONLY is set.
    PSYC_PARSE_CONTENT_END = 11,
    /// Content parsing done in one go, value contains the whole content.
    /// Used when PSYC_PARSE_ROUTING_ONLY is set.
    PSYC_PARSE_CONTENT = 12,
    /// Finished parsing packet.
    PSYC_PARSE_COMPLETE = 13,
} PsycParseRC;

/** PSYC packet parts. */
typedef enum {
    PSYC_PART_RESET = -1,
    PSYC_PART_ROUTING = 0,
    PSYC_PART_LENGTH = 1,
    PSYC_PART_CONTENT = 2,
    PSYC_PART_METHOD = 3,
    PSYC_PART_DATA = 4,
    PSYC_PART_END = 5,
} PsycPart;

/**
 * The return value definitions for the list parsing function.
 * @see psyc_parse_list()
 */
typedef enum {
    /// Error, no length is set for an element which is longer than PSYC_ELEM_SIZE_THRESHOLD.
    PSYC_PARSE_LIST_ERROR_ELEM_NO_LEN = -6,
    PSYC_PARSE_LIST_ERROR_ELEM_LENGTH = -5,
    PSYC_PARSE_LIST_ERROR_ELEM_TYPE = -4,
    PSYC_PARSE_LIST_ERROR_ELEM_START = -3,
    PSYC_PARSE_LIST_ERROR_TYPE = -2,
    PSYC_PARSE_LIST_ERROR = -1,
    /// Reached end of buffer.
    /// Buffer contains insufficient amount of data.
    /// Fill another buffer and concatenate it with the end of the current buffer,
    /// from the cursor position to the end.
    PSYC_PARSE_LIST_INSUFFICIENT = 1,
    /// Completed parsing the default type of the list.
    PSYC_PARSE_LIST_TYPE = 2,
    /// Start of an element is parsed but still not complete.
    PSYC_PARSE_LIST_ELEM_START = 3,
    /// Continuation of an incomplete element.
    PSYC_PARSE_LIST_ELEM_CONT = 4,
    /// Element parsing completed.
    PSYC_PARSE_LIST_ELEM_END = 5,
    /// Completed parsing a list element.
    PSYC_PARSE_LIST_ELEM = 6,
    /// Completed parsing the last element in the list.
    PSYC_PARSE_LIST_ELEM_LAST = 7,
    /// Reached end of buffer.
    PSYC_PARSE_LIST_END = 8,
} PsycParseListRC;

typedef enum {
    PSYC_LIST_PART_START = 0,
    PSYC_LIST_PART_TYPE = 1,
    PSYC_LIST_PART_ELEM_START = 2,
    PSYC_LIST_PART_ELEM_TYPE = 3,
    PSYC_LIST_PART_ELEM_LENGTH = 4,
    PSYC_LIST_PART_ELEM = 5,
} PsycListPart;

typedef enum {
    PSYC_PARSE_DICT_ERROR_VALUE = -9,
    PSYC_PARSE_DICT_ERROR_VALUE_LENGTH = -8,
    PSYC_PARSE_DICT_ERROR_VALUE_TYPE = -7,
    PSYC_PARSE_DICT_ERROR_VALUE_START = -6,
    PSYC_PARSE_DICT_ERROR_KEY = -5,
    PSYC_PARSE_DICT_ERROR_KEY_LENGTH = -4,
    PSYC_PARSE_DICT_ERROR_KEY_START = -3,
    PSYC_PARSE_DICT_ERROR_TYPE = -2,
    PSYC_PARSE_DICT_ERROR = -1,
    /// Reached end of buffer.
    /// Buffer contains insufficient amount of data.
    /// Fill another buffer and concatenate it with the end of the current buffer,
    /// from the cursor position to the end.
    PSYC_PARSE_DICT_INSUFFICIENT = 1,
    /// Completed parsing the default type of the dict.
    PSYC_PARSE_DICT_TYPE = 2,
    /// Start of a key is parsed but still not complete.
    PSYC_PARSE_DICT_KEY_START = 3,
    /// Continuation of an incomplete key.
    PSYC_PARSE_DICT_KEY_CONT = 4,
    /// Last continuation of a key.
    PSYC_PARSE_DICT_KEY_END = 5,
    /// Completed parsing a key in one go.
    PSYC_PARSE_DICT_KEY = 6,
    /// Start of a value is parsed but still not complete.
    PSYC_PARSE_DICT_VALUE_START = 7,
    /// Continuation of an incomplete value.
    PSYC_PARSE_DICT_VALUE_CONT = 8,
    /// Last continuation of a value.
    PSYC_PARSE_DICT_VALUE_END = 9,
    /// Completed parsing a value.
    PSYC_PARSE_DICT_VALUE = 10,
    /// Completed parsing the last value.
    PSYC_PARSE_DICT_VALUE_LAST = 11,
    /// Reached end of buffer.
    PSYC_PARSE_DICT_END = 12,
} PsycParseDictRC;

typedef enum {
    PSYC_DICT_PART_START = 0,
    PSYC_DICT_PART_TYPE = 1,
    PSYC_DICT_PART_KEY_START = 2,
    PSYC_DICT_PART_KEY_LENGTH = 3,
    PSYC_DICT_PART_KEY = 4,
    PSYC_DICT_PART_VALUE_START = 5,
    PSYC_DICT_PART_VALUE_TYPE = 6,
    PSYC_DICT_PART_VALUE_LENGTH = 7,
    PSYC_DICT_PART_VALUE = 8,
} PsycDictPart;

typedef enum {
    PSYC_PARSE_INDEX_ERROR_DICT = -6,
    PSYC_PARSE_INDEX_ERROR_DICT_LENGTH = -5,
    PSYC_PARSE_INDEX_ERROR_STRUCT = -4,
    PSYC_PARSE_INDEX_ERROR_LIST = -3,
    PSYC_PARSE_INDEX_ERROR_TYPE = -2,
    PSYC_PARSE_INDEX_ERROR = -1,
    /// Reached end of buffer.
    /// Buffer contains insufficient amount of data.
    /// Fill another buffer and concatenate it with the end of the current buffer,
    /// from the cursor position to the end.
    PSYC_PARSE_INDEX_INSUFFICIENT = 1,
    // Completed parsing a list index.
    PSYC_PARSE_INDEX_LIST = 3,
    // Completed parsing a list index at the end of the buffer.
    PSYC_PARSE_INDEX_LIST_LAST = 4,
    // Completed parsing a struct element name.
    PSYC_PARSE_INDEX_STRUCT = 5,
    // Completed parsing a struct element name at the end of the buffer.
    PSYC_PARSE_INDEX_STRUCT_LAST = 6,
    /// Start of a dict key is parsed but still not complete.
    PSYC_PARSE_INDEX_DICT_START = 7,
    /// Continuation of an incomplete dict key.
    PSYC_PARSE_INDEX_DICT_CONT = 8,
    /// Last continuation of a dict key.
    PSYC_PARSE_INDEX_DICT_END = 9,
    /// Completed parsing a dict key in one go.
    PSYC_PARSE_INDEX_DICT = 10,
    /// Reached end of buffer.
    PSYC_PARSE_INDEX_END = 11,
} PsycParseIndexRC;

typedef enum {
    PSYC_INDEX_PART_START = 0,
    PSYC_INDEX_PART_TYPE = 1,
    PSYC_INDEX_PART_LIST = 2,
    PSYC_INDEX_PART_STRUCT = 3,
    PSYC_INDEX_PART_DICT_LENGTH = 4,
    PSYC_INDEX_PART_DICT = 5,
} PsycIndexPart;

typedef enum {
    PSYC_PARSE_UPDATE_ERROR_VALUE = -24,
    PSYC_PARSE_UPDATE_ERROR_LENGTH = -23,
    PSYC_PARSE_UPDATE_ERROR_TYPE = -22,
    PSYC_PARSE_UPDATE_ERROR_OPER = -21,
    PSYC_PARSE_UPDATE_ERROR = -1,
    /// Reached end of buffer.
    /// Buffer contains insufficient amount of data.
    /// Fill another buffer and concatenate it with the end of the current buffer,
    /// from the cursor position to the end.
    PSYC_PARSE_UPDATE_INSUFFICIENT = 1,

    // Completed parsing a list index.
    PSYC_PARSE_UPDATE_INDEX_LIST = 3,
    // Completed parsing a struct element name.
    PSYC_PARSE_UPDATE_INDEX_STRUCT = 5,
    /// Start of a dict key is parsed but still not complete.
    PSYC_PARSE_UPDATE_INDEX_DICT_START = 7,
    /// Continuation of an incomplete dict key.
    PSYC_PARSE_UPDATE_INDEX_DICT_CONT = 8,
    /// Last continuation of a dict key.
    PSYC_PARSE_UPDATE_INDEX_DICT_END = 9,
    /// Completed parsing a dict key in one go.
    PSYC_PARSE_UPDATE_INDEX_DICT = 10,

    /// Completed parsing the type.
    PSYC_PARSE_UPDATE_TYPE = 21,
    /// Completed parsing the type and reached end of buffer.
    PSYC_PARSE_UPDATE_TYPE_END = 22,
    /// Start of the value is parsed but still not complete.
    PSYC_PARSE_UPDATE_VALUE_START = 23,
    /// Continuation of incomplete value.
    PSYC_PARSE_UPDATE_VALUE_CONT = 24,
    /// Last continuation of the value.
    PSYC_PARSE_UPDATE_VALUE_END = 25,
    /// Completed parsing the value in one go.
    PSYC_PARSE_UPDATE_VALUE = 26,
    /// Reached end of buffer.
    PSYC_PARSE_UPDATE_END = 27,
} PsycParseUpdateRC;

typedef enum {
    PSYC_UPDATE_PART_START = 0,

    PSYC_UPDATE_INDEX_PART_TYPE = 1,
    PSYC_UPDATE_INDEX_PART_LIST = 2,
    PSYC_UPDATE_INDEX_PART_STRUCT = 3,
    PSYC_UPDATE_INDEX_PART_DICT_LENGTH = 4,
    PSYC_UPDATE_INDEX_PART_DICT = 5,

    PSYC_UPDATE_PART_TYPE = 12,
    PSYC_UPDATE_PART_LENGTH = 13,
    PSYC_UPDATE_PART_VALUE = 14,
} PsycUpdatePart;

/**
 * Struct for keeping parser state.
 */
typedef struct {
    PsycString buffer;		///< Buffer with data to be parsed.
    size_t cursor;		///< Current position in buffer.
    size_t startc;		///< Position where the parsing would be resumed.

    size_t routinglen;		///< Length of routing part parsed so far.
    size_t contentlen;		///< Expected length of the content.
    size_t content_parsed;	///< Number of bytes parsed from the content so far.
    size_t valuelen;		///< Expected length of the value.
    size_t value_parsed;	///< Number of bytes parsed from the value so far.

    PsycPart part;		///< Part of the packet being parsed currently.
    uint8_t flags;		///< Flags for the parser, see PsycParseFlag.
    uint8_t contentlen_found;	///< Is there a length given for this packet?
    uint8_t valuelen_found;	///< Is there a length given for this modifier?
} PsycParseState;

/**
 * Struct for keeping list parser state.
 */
typedef struct {
    PsycString buffer;		///< Buffer with data to be parsed.
    size_t cursor;		///< Current position in buffer.
    size_t startc;		///< Line start position.

    PsycString type;		///< List type.
    size_t elemlen;		///< Expected length of the elem.
    size_t elem_parsed;		///< Number of bytes parsed from the elem so far.

    PsycListPart part;		///< Part of the list being parsed currently.
    uint8_t elemlen_found;	///< Is there a length given for this element?
} PsycParseListState;

/**
 * Struct for keeping dict parser state.
 */
typedef struct {
    PsycString buffer;		///< Buffer with data to be parsed.
    size_t cursor;		///< Current position in buffer.
    size_t startc;		///< Line start position.

    size_t elemlen;		///< Expected length of the key/value.
    size_t elem_parsed;		///< Number of bytes parsed from the key/value so far.

    PsycDictPart part;		///< Part of the dict being parsed currently.
    uint8_t elemlen_found;	///< Is there a length given for this key/value?
} PsycParseDictState;

/**
 * Struct for keeping index parser state.
 */
typedef struct {
    PsycString buffer;		///< Buffer with data to be parsed.
    size_t cursor;		///< Current position in buffer.
    size_t startc;		///< Position where the parsing would be resumed.

    size_t elemlen;		///< Expected length of an element.
    size_t elem_parsed;		///< Number of bytes parsed from the elem so far.

    PsycIndexPart part;		///< Part of the packet being parsed currently.
    uint8_t elemlen_found;	///< Is there a length given for this element?
} PsycParseIndexState;

/**
 * Struct for keeping update modifier parser state.
 */
typedef struct {
    PsycString buffer;		///< Buffer with data to be parsed.
    size_t cursor;		///< Current position in buffer.
    size_t startc;		///< Position where the parsing would be resumed.

    size_t elemlen;		///< Expected length of an element.
    size_t elem_parsed;		///< Number of bytes parsed from the elem so far.

    PsycUpdatePart part;	///< Part of the packet being parsed currently.
    uint8_t elemlen_found;	///< Is there a length given for this element?
} PsycParseUpdateState;

/**
 * Initializes the state struct.
 *
 * @param state Pointer to the state struct that should be initialized.
 * @param flags Flags to be set for the parser, see PsycParseFlag.
 * @see PsycParseFlag
 */
inline void
psyc_parse_state_init (PsycParseState *state, uint8_t flags)
{
    memset(state, 0, sizeof(PsycParseState));
    state->flags = flags;

    if (flags & PSYC_PARSE_START_AT_CONTENT)
	state->part = PSYC_PART_CONTENT;
}

/**
 * Sets a new buffer in the parser state struct with data to be parsed.
 *
 * This function does NOT copy the buffer. It will parse whatever is
 * at the memory pointed to by buffer.
 *
 * @param state Pointer to the initialized state of the parser
 * @param buffer pointer to the data that should be parsed
 * @param length length of the data in bytes
 * @see PsycString
 */
inline void
psyc_parse_buffer_set (PsycParseState *state, const char *buffer, size_t length)
{
    state->buffer = (PsycString) {length, (char*)buffer};
    state->cursor = 0;

    if (state->flags & PSYC_PARSE_START_AT_CONTENT) {
	state->contentlen = length;
	state->contentlen_found = PSYC_TRUE;
    }
}

/**
 * Initializes the list parser state.
 */
inline void
psyc_parse_list_state_init (PsycParseListState *state)
{
    memset(state, 0, sizeof(PsycParseListState));
}

/**
 * Sets a new buffer in the list parser state struct with data to be parsed.
 */
inline void
psyc_parse_list_buffer_set (PsycParseListState *state,
			    const char *buffer, size_t length)
{
    state->buffer = (PsycString) {length, (char*)buffer};
    state->cursor = 0;
}

/**
 * Initializes the dict parser state.
 */
inline void
psyc_parse_dict_state_init (PsycParseDictState *state)
{
    memset(state, 0, sizeof(PsycParseDictState));
}

/**
 * Sets a new buffer in the dict parser state struct with data to be parsed.
 */
inline void
psyc_parse_dict_buffer_set (PsycParseDictState *state,
			     const char *buffer, size_t length)
{
    state->buffer = (PsycString) {length, (char*)buffer};
    state->cursor = 0;
}

/**
 * Initializes the index parser state.
 */
inline void
psyc_parse_index_state_init (PsycParseIndexState *state)
{
    memset(state, 0, sizeof(PsycParseIndexState));
}

/**
 * Sets a new buffer in the index parser state struct with data to be parsed.
 */
inline void
psyc_parse_index_buffer_set (PsycParseIndexState *state,
			     const char *buffer, size_t length)
{
    state->buffer = (PsycString) {length, (char*)buffer};
    state->cursor = 0;
}

/**
 * Initializes the update modifier parser state.
 */
inline void
psyc_parse_update_state_init (PsycParseUpdateState *state)
{
    memset(state, 0, sizeof(PsycParseUpdateState));
}

/**
 * Sets a new buffer in the update modifier parser state struct with data to be parsed.
 */
inline void
psyc_parse_update_buffer_set (PsycParseUpdateState *state,
			     const char *buffer, size_t length)
{
    state->buffer = (PsycString) {length, (char*)buffer};
    state->cursor = 0;
}

inline size_t
psyc_parse_content_length (PsycParseState *state)
{
    return state->contentlen;
}

inline PsycBool
psyc_parse_content_length_found (PsycParseState *state)
{
    return (PsycBool) state->contentlen_found;
}

inline size_t
psyc_parse_value_length (PsycParseState *state)
{
    return state->valuelen;
}

inline PsycBool
psyc_parse_value_length_found (PsycParseState *state)
{
    return (PsycBool) state->valuelen_found;
}

inline size_t
psyc_parse_cursor (PsycParseState *state)
{
    return state->cursor;
}

inline size_t
psyc_parse_buffer_length (PsycParseState *state)
{
    return state->buffer.length;
}

inline size_t
psyc_parse_remaining_length (PsycParseState *state)
{
    return state->buffer.length - state->cursor;
}

inline const char *
psyc_parse_remaining_buffer (PsycParseState *state)
{
    return state->buffer.data + state->cursor;
}

/**
 * Parse PSYC packets.
 *
 * This function parses a full or partial PSYC packet while keeping parsing
 * state in a state variable that you have to pass in every time, and returns
 * whenever a modifier or the body is found. See PsycParseRC for the possible
 * return codes. When it returns oper, name & value will point to the respective
 * parts of the buffer, no memory allocation is done.
 *
 * @param state An initialized PsycParseState.
 * @param oper In case of a modifier it will be set to the operator.
 * @param name In case of a modifier it will point to the name,
 *             in case of the body it will point to the method.
 * @param value In case of a modifier it will point to the value,
 *              in case of the body it will point to the data.
 */
#ifdef __INLINE_PSYC_PARSE
inline
#endif
PsycParseRC
psyc_parse (PsycParseState *state, char *oper,
	    PsycString *name, PsycString *value);

/**
 * List parser.
 *
 * This function parses a _list modifier value and returns one element a time
 * while keeping parsing state in a state variable that you have to pass in
 * every time. When it returns elem will point to the next element in value, no
 * memory allocation is done.
 *
 * @param state An initialized PsycParseListState.
 * @param elem It will point to the next element in the list.
 */
#ifdef __INLINE_PSYC_PARSE
inline
#endif
PsycParseListRC
psyc_parse_list (PsycParseListState *state, PsycString *type, PsycString *elem);

#ifdef __INLINE_PSYC_PARSE
inline
#endif
PsycParseDictRC
psyc_parse_dict (PsycParseDictState *state, PsycString *type, PsycString *elem);

#ifdef __INLINE_PSYC_PARSE
inline
#endif
PsycParseIndexRC
psyc_parse_index (PsycParseIndexState *state, PsycString *idx);

#ifdef __INLINE_PSYC_PARSE
inline
#endif
PsycParseUpdateRC
psyc_parse_update (PsycParseUpdateState *state, char *oper, PsycString *value);

inline size_t
psyc_parse_int (const char *value, size_t len, int64_t *n)
{
    size_t c = 0;
    uint8_t neg = 0;

    if (!value)
	return c;

    if (value[0] == '-')
	neg = ++c;

    *n = 0;
    while (c < len && value[c] >= '0' && value[c] <= '9')
	*n = 10 * *n + (value[c++] - '0');

    if (c != len)
	return c;

    if (neg)
	*n = 0 - *n;

    return c;
}

inline size_t
psyc_parse_uint (const char *value, size_t len, uint64_t *n)
{
    size_t c = 0;
    if (!value)
	return c;

    *n = 0;
    while (c < len && value[c] >= '0' && value[c] <= '9')
	*n = 10 * *n + (value[c++] - '0');

    return c;
}

inline size_t
psyc_parse_list_index (const char *value, size_t len, int64_t *n)
{
    if (!value || len == 0 || value[0] != '#')
	return 0;
    return psyc_parse_int(value + 1, len, n) + 1;
}

/**
 * Determines if the argument is a glyph.
 * Glyphs are: : = + - ? !
 */
inline PsycBool
psyc_is_oper (char g)
{
    switch (g) {
    case ':':
    case '=':
    case '+':
    case '-':
    case '@':
    case '?':
    case '!':
	return PSYC_TRUE;
    default:
	return PSYC_FALSE;
    }
}

/**
 * Determines if the argument is numeric.
 */
inline char
psyc_is_numeric (char c)
{
    return c >= '0' && c <= '9';
}

/**
 * Determines if the argument is alphabetic.
 */
inline char
psyc_is_alpha (char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/**
 * Determines if the argument is alphanumeric.
 */
inline char
psyc_is_alpha_numeric (char c)
{
    return psyc_is_alpha(c) || psyc_is_numeric(c);
}

/**
 * Determines if the argument is a keyword character.
 * Keyword characters are: alphanumeric and _
 */
inline char
psyc_is_kw_char (char c)
{
    return psyc_is_alpha_numeric(c) || c == '_';
}

/**
 * Determines if the argument is a name character.
 * Name characters are: see opaque_part in RFC 2396
 */
inline char
psyc_is_name_char (char c)
{
    return psyc_is_alpha(c) || (c >= '$' && c <= ';')
	|| c == '_' || c == '!' || c == '?' || c == '=' || c == '@' || c == '~';
}

/**
 * Determines if the argument is a hostname character.
 * Hostname characters are: alphanumeric and -
 */
inline char
psyc_is_host_char (char c)
{
    return psyc_is_alpha_numeric(c) || c == '.' || c == '-';
}

/**
 * Parse variable name or method name.
 * It should contain one or more keyword characters.
 * @return Number of characters parsed.
 */
inline size_t
psyc_parse_keyword (const char *data, size_t len)
{
    size_t c = 0;
    while (c < len && psyc_is_kw_char(data[c++]));
    return c > 0 ? c - 1 : 0;
}

/** @} */ // end of parse group

#endif
