module psyc.parser;

import psyc.common;

/**
 * @file d/psyc/parser.d
 * @brief D Interface for various PSYC parser functions.
 *
 * All parsing functions and the definitions they use are
 * defined in this file.
*/

/**
 * @defgroup parsing-d Parsing Functions
 *
 * This module contains all parsing functions.
 * @{
 */

extern (C):

enum ParseFlag
{
	PARSE_HEADER_ONLY = 1,
}

/**
 * The return value definitions for the packet parsing function.
 * @see parse()
 */
enum ParseRC
{
	PARSE_ERROR_END = -9,
	PARSE_ERROR_BODY = -8,
	PARSE_ERROR_METHOD = -7,
	PARSE_ERROR_MOD_NL = -6,
	PARSE_ERROR_MOD_LEN = -5,
	PARSE_ERROR_MOD_TAB = -4,
	PARSE_ERROR_MOD_NAME = -3,
	PARSE_ERROR_LENGTH = -2,
	PARSE_ERROR = -1,
	PARSE_SUCCESS = 0,
/// Buffer contains insufficient amount of data.
/// Fill another buffer and concatenate it with the end of the current buffer,
/// from the cursor position to the end.
	PARSE_INSUFFICIENT = 1,
/// Routing modifier parsing done.
/// Operator, name & value contains the respective parts.
	PARSE_ROUTING = 2,
/// Entity modifier parsing done.
/// Operator, name & value contains the respective parts.
	PARSE_ENTITY = 3,
/// Entity modifier parsing is incomplete.
/// Operator & name are complete, value is incomplete.
	PARSE_ENTITY_INCOMPLETE = 4,
/// Body parsing done, name contains method, value contains body.
	PARSE_BODY = 5,
/// Body parsing is incomplete, name contains method, value contains part of the body.
	PARSE_BODY_INCOMPLETE = 6,
/// Reached end of packet, parsing done.
	PARSE_COMPLETE = 7,
/// Binary value parsing incomplete, used internally.
	PARSE_INCOMPLETE = 8,
}

/**
 * The return value definitions for the list parsing function.
 * @see parseList()
 */
enum ParseListRC
{
	PARSE_LIST_ERROR_DELIM = -5,
	PARSE_LIST_ERROR_LEN = -4,
	PARSE_LIST_ERROR_TYPE = -3,
	PARSE_LIST_ERROR_NAME = -2,
	PARSE_LIST_ERROR= -1,
/// Completed parsing a list element.
	PARSE_LIST_ELEM = 1,
/// Reached end of buffer.
	PARSE_LIST_END = 2,
/// Binary list is incomplete.
	PARSE_LIST_INCOMPLETE = 3,
}

/**
 * Struct for keeping parser state.
 */
struct ParseState
{
	size_t cursor; ///< current position in buffer
	size_t startc; ///< position where the parsing would be resumed
	String buffer; ///< buffer with data to be parsed
	ubyte flags; ///< flags for the parser, see ParseFlag
	Part part; ///< part of the packet being parsed currently

	size_t routingLength; ///< length of routing part parsed so far
	size_t contentParsed; ///< number of bytes parsed from the content so far
	size_t contentLength; ///< expected length of the content
	Bool contentLengthFound; ///< is there a length given for this packet?
	size_t valueParsed; ///< number of bytes parsed from the value so far
	size_t valueLength; ///< expected length of the value
}

/**
 * Struct for keeping list parser state.
 */
struct ParseListState
{
	size_t cursor; ///< current position in buffer
	size_t startc; ///< line start position
	String buffer;
	ListType type; ///< list type

	size_t elemParsed; ///< number of bytes parsed from the elem so far
	size_t elemLength; ///< expected length of the elem
}

/**
 * Initiates the state struct.
 *
 * @param state Pointer to the state struct that should be initiated.
 */
void psyc_initParseState (ParseState* state);

/**
 * Initiates the state struct with flags.
 *
 * @param state Pointer to the state struct that should be initiated.
 * @param flags Flags to be set for the parser, see ParseFlag.
 */
void psyc_initParseState2 (ParseState* state, ubyte flags);

/**
 * Initiates the list state struct.
 *
 * @param state Pointer to the list state struct that should be initiated.
 */
void psyc_initParseListState (ParseListState* state);

void psyc_nextParseBuffer (ParseState* state, String newBuf);

void psyc_nextParseListBuffer (ParseListState* state, String newBuf);

size_t psyc_getContentLength (ParseState* s);

/**
 * Parse PSYC packets.
 *
 * Generalized line-based packet parser.
 *
 * @param state An initialized ParseState
 * @param oper  A pointer to a character. In case of a variable, it will
 *              be set to the operator of that variable
 * @param name A pointer to a String. It will point to the name of
 *             the variable or method and its length will be set accordingly
 * @param value A pointer to a String. It will point to the
 *              value/body the variable/method and its length will be set accordingly
 */
ParseRC psyc_parse(ParseState* state, char* oper, String* name, String* value);

/**
 * List value parser.
 */
ParseListRC psyc_parseList(ParseListState* state, String *name, String* value, String* elem);

/** @} */ // end of parsing group
