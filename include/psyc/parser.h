/**
 * @file psyc/parser.h
 * @brief Interface for various PSYC parser functions.
 *
 * All parsing functions and the definitions they use are
 * defined in this file.
 */

/**
 * @defgroup parser Parsing Functions
 *
 * This module contains all parsing functions.
 * @{
 */

#ifndef PSYC_PARSER_H
# define PSYC_PARSER_H

#include <stdint.h>
#include <string.h>
#include <psyc.h>

typedef enum
{
	/// Parse only the header
	PSYC_PARSE_ROUTING_ONLY = 1,
	/// Expects only the content part of a packet. The buffer should contain the whole content in this case.
	PSYC_PARSE_START_AT_CONTENT = 2,
} psycParseFlag;

/**
 * The return value definitions for the packet parsing function.
 * @see psyc_parse()
 */
typedef enum
{
	PSYC_PARSE_ERROR_END = -9,
	PSYC_PARSE_ERROR_BODY = -8,
	PSYC_PARSE_ERROR_METHOD = -7,
	PSYC_PARSE_ERROR_MOD_NL = -6,
	PSYC_PARSE_ERROR_MOD_LEN = -5,
	PSYC_PARSE_ERROR_MOD_TAB = -4,
	PSYC_PARSE_ERROR_MOD_NAME = -3,
	PSYC_PARSE_ERROR_LENGTH = -2,
	PSYC_PARSE_ERROR = -1,
	PSYC_PARSE_SUCCESS = 0,
/// Buffer contains insufficient amount of data.
/// Fill another buffer and concatenate it with the end of the current buffer,
/// from the cursor position to the end.
	PSYC_PARSE_INSUFFICIENT = 1,
/// Routing modifier parsing done.
/// Operator, name & value contains the respective parts.
	PSYC_PARSE_ROUTING = 2,
/// Entity modifier parsing done.
/// Operator, name & value contains the respective parts.
	PSYC_PARSE_ENTITY = 3,
/// Entity modifier parsing is incomplete.
/// Operator & name are complete, value is incomplete.
	PSYC_PARSE_ENTITY_INCOMPLETE = 4,
/// Body parsing done, name contains method, value contains body.
	PSYC_PARSE_BODY = 5,
/// Body parsing is incomplete, name contains method, value contains part of the body.
	PSYC_PARSE_BODY_INCOMPLETE = 6,
/// Content parsing done, value contains the whole content.
/// Used when PSYC_PARSE_ROUTING_ONLY is set.
	PSYC_PARSE_CONTENT = 5,
/// Content parsing is incomplete, value contains part of content.
/// Used when PSYC_PARSE_ROUTING_ONLY is set.
	PSYC_PARSE_CONTENT_INCOMPLETE = 6,
/// Reached end of packet, parsing done.
	PSYC_PARSE_COMPLETE = 7,
/// Binary value parsing incomplete, used internally.
	PSYC_PARSE_INCOMPLETE = 8,
} psycParseRC;

/**
 * The return value definitions for the list parsing function.
 * @see psyc_parseList()
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
} psycParseListRC;

/**
 * Struct for keeping parser state.
 */
typedef struct
{
	size_t cursor; ///< current position in buffer
	size_t startc; ///< position where the parsing would be resumed
	psycString buffer; ///< buffer with data to be parsed
	uint8_t flags; ///< flags for the parser, see psycParseFlag
	psycPart part; ///< part of the packet being parsed currently

	size_t routingLength; ///< length of routing part parsed so far
	size_t contentParsed; ///< number of bytes parsed from the content so far
	size_t contentLength; ///< expected length of the content
	psycBool contentLengthFound; ///< is there a length given for this packet?
	size_t valueParsed; ///< number of bytes parsed from the value so far
	size_t valueLength; ///< expected length of the value
	psycBool valueLengthFound; ///< is there a length given for this modifier?
} psycParseState;

/**
 * Struct for keeping list parser state.
 */
typedef struct
{
	size_t cursor; ///< current position in buffer
	size_t startc; ///< line start position
	psycString buffer;
	psycListType type; ///< list type

	size_t elemParsed; ///< number of bytes parsed from the elem so far
	size_t elemLength; ///< expected length of the elem
} psycParseListState;

/**
 * Initializes the state struct.
 *
 * @param state Pointer to the state struct that should be initialized.
 */
static inline
void psyc_initParseState (psycParseState *state)
{
	memset(state, 0, sizeof(psycParseState));
}

/**
 * Initializes the state struct with flags.
 *
 * @param state Pointer to the state struct that should be initialized.
 * @param flags Flags to be set for the parser, see psycParseFlag.
 */
static inline
void psyc_initParseState2 (psycParseState *state, uint8_t flags)
{
	memset(state, 0, sizeof(psycParseState));
	state->flags = flags;

	if (flags & PSYC_PARSE_START_AT_CONTENT)
		state->part = PSYC_PART_CONTENT;
}

/**
 * Sets a new buffer in the parser state struct with data to be parsed.
 *
 * @param state Pointer to the initialized state of the parser
 * @param buffer the buffer that should be parsed now
 * @see psycString
 */
static inline
void psyc_setParseBuffer (psycParseState *state, psycString buffer)
{
	state->buffer = buffer;
	state->cursor = 0;

	if (state->flags & PSYC_PARSE_START_AT_CONTENT)
	{
		state->contentLength = buffer.length;
		state->contentLengthFound = PSYC_TRUE;
	}
}

/**
 * Sets a new buffer in the parser state struct with data to be parsed.
 *
 * @param state Pointer to the initialized state of the parser
 * @param buffer pointer to the data that should be parsed
 * @param length length of the data in bytes 
 * @see psycString
 */
static inline
void psyc_setParseBuffer2 (psycParseState *state, char *buffer, size_t length)
{
	psyc_setParseBuffer(state, psyc_newString(buffer, length));
}

/**
 * Initializes the list state struct.
 *
 * @param state Pointer to the list state struct that should be initialized.
 */
static inline
void psyc_initParseListState (psycParseListState *state)
{
	memset(state, 0, sizeof(psycParseListState));
}

/**
 * Sets a new buffer in the list parser state struct with data to be parsed.
 */
static inline
void psyc_setParseListBuffer (psycParseListState *state, psycString buffer)
{
	state->buffer = buffer;
	state->cursor = 0;
}

static inline
void psyc_setParseListBuffer2 (psycParseListState *state, char *buffer, size_t length)
{
	psyc_setParseListBuffer(state, psyc_newString(buffer, length));
}

static inline
size_t psyc_getParseContentLength (psycParseState *state)
{
	return state->contentLength;
}

static inline
psycBool psyc_isParseContentLengthFound (psycParseState *state)
{
	return state->contentLengthFound;
}

static inline
size_t psyc_getParseValueLength (psycParseState *state)
{
	return state->valueLength;
}

static inline
psycBool psyc_isParseValueLengthFound (psycParseState *state)
{
	return state->valueLengthFound;
}

static inline
size_t psyc_getParseCursor (psycParseState *state)
{
	return state->cursor;
}

static inline
size_t psyc_getParseBufferLength (psycParseState *state)
{
	return state->buffer.length;
}

static inline
size_t psyc_getParseRemainingLength (psycParseState *state)
{
	return state->buffer.length - state->cursor;
}

static inline
const char * psyc_getParseRemainingBuffer (psycParseState *state)
{
	return state->buffer.ptr + state->cursor;
}

/**
 * Parse PSYC packets.
 *
 * Generalized line-based packet parser.
 *
 * @param state An initialized psycParseState
 * @param operator A pointer to a character. In case of a variable, it will
 *                 be set to the operator of that variable
 * @param name A pointer to a psycString. It will point to the name of
 *             the variable or method and its length will be set accordingly
 * @param value A pointer to a psycString. It will point to the
 *              value/body the variable/method and its length will be set accordingly
 */
psycParseRC psyc_parse (psycParseState *state, char *oper,
                        psycString *name, psycString *value);

/**
 * List value parser.
 */
psycParseListRC psyc_parseList (psycParseListState *state, psycString *name,
                                psycString *value, psycString *elem);

#endif // PSYC_PARSER_H

/** @} */ // end of parser group
