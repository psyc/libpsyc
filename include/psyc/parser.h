/**
 * @file psyc/parser.h
 * @brief Interface for various PSYC parser functions.
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
	/// Parse only the header
	PSYC_PARSE_HEADER_ONLY = 1,
	/// Expects only the content part of a packet. The length of the content must fit exactly in this case
	PSYC_PARSE_BEGIN_AT_CONTENT = 2,
} psycParseFlag;

/**
 * The return value definitions for the packet parsing function.
 * @see psyc_parse()
 */
typedef enum
{
	/// Invalid combination of flags
	PSYC_PARSE_ERROR_INVALID_FLAGS = -10,
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
 * Initiates the state struct.
 *
 * @param state Pointer to the state struct that should be initiated.
 */
inline void psyc_initParseState (psycParseState* state);

/**
 * Initiates the state struct with flags.
 *
 * @param state Pointer to the state struct that should be initiated.
 * @param flags Flags to be set for the parser, see psycParseFlag.
 */
inline void psyc_initParseState2 (psycParseState* state, uint8_t flags);

/**
 * Initiates the list state struct.
 *
 * @param state Pointer to the list state struct that should be initiated.
 */
inline void psyc_initParseListState (psycParseListState* state);

inline void psyc_nextParseBuffer (psycParseState* state, psycString newBuf);

inline void psyc_nextParseListBuffer (psycParseListState* state, psycString newBuf);

inline size_t psyc_getContentLength (psycParseState* s);

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
psycParseRC psyc_parse(psycParseState* state, char* oper, psycString* name, psycString* value);

/**
 * List value parser.
 */
psycParseListRC psyc_parseList(psycParseListState* state, psycString *name, psycString* value, psycString* elem);

#endif // PSYC_PARSER_H

/** @} */ // end of parsing group
