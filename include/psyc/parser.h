/**
 * @file psyc/parser.h
 * @brief Interface for various psyc parser functions.
 *
 * All parsing functions and the definitions they use are
 * defined in this file.
*/

/**
 * @defgroup parsing Parsing Functions
 *
 * This module contains all parsing functions.
 * @{
 */

#ifndef PSYC_PARSER_H
# define PSYC_PARSER_H

#include <stdint.h>
#include <string.h>

typedef enum
{
	PSYC_PARSE_HEADER_ONLY = 1,
} PSYC_ParseFlag;

/**
 * The return value definitions for the packet parsing function.
 * @see PSYC_parse()
 */
typedef enum
{
	PSYC_PARSE_ERROR_END = -7,
	PSYC_PARSE_ERROR_METHOD = -6,
	PSYC_PARSE_ERROR_VAR_LEN = -5,
	PSYC_PARSE_ERROR_VAR_TAB = -4,
	PSYC_PARSE_ERROR_VAR_NAME = -3,
	PSYC_PARSE_ERROR_LENGTH = -2,
	PSYC_PARSE_ERROR = -1,
	PSYC_PARSE_SUCCESS = 0,
/// Buffer contains insufficient amount of data.
/// Fill another buffer and concatenate it with the end of the current buffer,
/// from the cursor position to the end.
	PSYC_PARSE_INSUFFICIENT = 1,
/// Routing variable parsing done.
/// Modifier, name & value contains the respective parts.
	PSYC_PARSE_ROUTING = 2,
/// Entity variable parsing done.
/// Modifier, name & value contains the respective parts.
	PSYC_PARSE_ENTITY = 3,
/// Entity variable parsing is incomplete.
/// Modifier & name are complete, value is incomplete.
	PSYC_PARSE_ENTITY_INCOMPLETE = 4,
/// Body parsing done, name contains method, value contains body.
	PSYC_PARSE_BODY = 5,
/// Body parsing is incomplete, name contains method, value contains part of the body.
	PSYC_PARSE_BODY_INCOMPLETE = 6,
/// Reached end of packet, parsing done.
	PSYC_PARSE_COMPLETE = 7,
/// Binary value parsing incomplete, used internally.
	PSYC_PARSE_INCOMPLETE = 8,
} PSYC_ParseRC;

/**
 * The return value definitions for the list parsing function.
 * @see PSYC_parseList()
 */
typedef enum
{
	PSYC_PARSE_LIST_ERROR_DELIM = -5,
	PSYC_PARSE_LIST_ERROR_LEN = -4,
	PSYC_PARSE_LIST_ERROR_TYPE = -3,
	PSYC_PARSE_LIST_ERROR_NAME = -2,
	PSYC_PARSE_LIST_ERROR= -1,
/// Completed parsing a list element.
	PSYC_PARSE_LIST_ELEM = 1,
/// Reached end of buffer.
	PSYC_PARSE_LIST_END = 2,
/// Binary list is incomplete.
	PSYC_PARSE_LIST_INCOMPLETE = 3,
} PSYC_ParseListRC;

/**
 * Struct for keeping parser state.
 */
typedef struct
{
	size_t cursor; ///< current position in buffer
	size_t startc; ///< position where the parsing would be resumed
	PSYC_Array buffer; ///< buffer with data to be parsed
	uint8_t flags; ///< flags for the parser, see PSYC_ParseFlag
	PSYC_Part part; ///< part of the packet being parsed currently

	size_t contentParsed; ///< number of bytes parsed from the content so far
	size_t contentLength; ///< expected length of the content
	PSYC_Bool contentLengthFound; ///< is there a length given for this packet?
	size_t valueParsed; ///< number of bytes parsed from the value so far
	size_t valueLength; ///< expected length of the value
} PSYC_ParseState;

/**
 * Struct for keeping list parser state.
 */
typedef struct
{
	size_t cursor; ///< current position in buffer
	size_t startc; ///< line start position
	PSYC_Array buffer;
	PSYC_ListType type; ///< list type

	size_t elemParsed; ///< number of bytes parsed from the elem so far
	size_t elemLength; ///< expected length of the elem
} PSYC_ParseListState;

/**
 * Shortcut for creating an array.
 *
 * @param memory Pointer to the buffer.
 * @param length Length of that buffer.
 *
 * @return An instance of the PSYC_Array struct.
 */
inline PSYC_Array PSYC_createArray (const char* memory, size_t length)
{
	PSYC_Array arr = {length, memory};
	return arr;
}

/**
 * Initiates the state struct with flags.
 *
 * @param state Pointer to the state struct that should be initiated.
 * @param flags Flags to be set for the parser, see PSYC_ParseFlag.
 */
inline void PSYC_initParseState2 (PSYC_ParseState* state, uint8_t flags)
{
	memset(state, 0, sizeof(PSYC_ParseState));
	state->flags = flags;
}

/**
 * Initiates the state struct.
 *
 * @param state Pointer to the state struct that should be initiated.
 */
inline void PSYC_initParseState (PSYC_ParseState* state)
{
	memset(state, 0, sizeof(PSYC_ParseState));
}

/**
 * Initiates the list state struct.
 *
 * @param state Pointer to the list state struct that should be initiated.
 */
inline void PSYC_initParseListState (PSYC_ParseListState* state)
{
	memset(state, 0, sizeof(PSYC_ParseListState));
}

inline void PSYC_nextParseBuffer (PSYC_ParseState* state, PSYC_Array newBuf)
{
	state->buffer = newBuf;
	state->cursor = 0;
}

inline void PSYC_nextParseListBuffer (PSYC_ParseListState* state, PSYC_Array newBuf)
{
	state->buffer = newBuf;
	state->cursor = 0;
}

inline size_t PSYC_getContentLength (PSYC_ParseState* s)
{
	return s->contentLength;
}

/**
 * Parse PSYC packets.
 *
 * Generalized line-based packet parser.
 *
 * @param state An initialized PSYC_ParseState
 * @param modifier A pointer to a character. In case of a variable, it will
 *                 be set to the modifier of that variable
 * @param name A pointer to a PSYC_Array. It will point to the name of
 *             the variable or method and its length will be set accordingly
 * @param value A pointer to a PSYC_Array. It will point to the
 *              value/body the variable/method and its length will be set accordingly
 */
PSYC_ParseRC PSYC_parse(PSYC_ParseState* state, char* modifier, PSYC_Array* name, PSYC_Array* value);

/**
 * List value parser.
 */
PSYC_ParseListRC PSYC_parseList(PSYC_ParseListState* state, PSYC_Array *name, PSYC_Array* value, PSYC_Array* elem);

#endif // PSYC_PARSER_H

/** @} */ // end of parsing group
