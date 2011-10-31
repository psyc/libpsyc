#ifndef PSYC_PARSE_H

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
 * psycParseState state;
 * psyc_parse_state_init(&state, flags);
 * @endcode
 *
 * With the flags parameter you can fine-tune what
 * part of the packet should be parsed. @see psycParseFlag
 *
 * Next, you have to tell the parser what it should parse. Assuming the variable
 * raw_data points to our packet and raw_len contains the length, you can pass
 * it to the parser as follows:
 *
 * @code
 * char* raw_data; // points to our (possibly incomplete) packet
 * size_t raw_len; // how many bytes of data
 *
 * psyc_parse_buffer_set(&state, raw_data, raw_len); // state is our initialized state from before
 * @endcode
 *
 * Now the the variables that will save the output of the parser need to be
 * declared:
 *
 * @code
 * psycString name,  // Name of the variable or method
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
 * 			        name.length, name.ptr,
 * 			        value.length, value.ptr,
 * 			        oper);
 * 			// Note that the .ptr member still points at your original buffer. If
 * 			// you want to reuse that buffer for the next packet, you better copy it
 * 			// before passing it to the parser or you copy each variable now.
 * 			break;
 * 		case PSYC_PARSE_BODY: // it is the method and the body of the packet.
 * 			printf("Method Name: %.*s  Body: %.*s\n",
 * 			        name.length, name.ptr,    // name of the method
 * 			        value.length, value.ptr); // value of the body
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
 * codes in psycParseRC and their explanations. @see psycParseRC
 */

/** @{ */ // begin of parser group

#include <stdint.h>
#include <string.h>
#include <psyc.h>

typedef enum {
	/// Default Flag. Parse everything.
	PSYC_PARSE_ALL = 0,
	/// Parse only the header
	PSYC_PARSE_ROUTING_ONLY = 1,
	/// Parse only the content.
	/// Parsing starts at the content and the content must be complete.
	PSYC_PARSE_START_AT_CONTENT = 2,
} psycParseFlag;

/**
 * The return value definitions for the packet parsing function.
 * @see psyc_parse()
 */
typedef enum {
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
	/// Start of an incomplete entity modifier.
	/// Operator & name are complete, value is incomplete.
	PSYC_PARSE_ENTITY_START = 3,
	/// Continuation of an incomplete entity modifier.
	PSYC_PARSE_ENTITY_CONT = 4,
	/// End of an incomplete entity modifier.
	PSYC_PARSE_ENTITY_END = 5,
	/// Entity modifier parsing done in one go.
	/// Operator, name & value contains the respective parts.
	PSYC_PARSE_ENTITY = 6,
	/// Start of an incomplete body.	
	/// Name contains method, value contains part of the body.
	/// Used when packet length is given
	PSYC_PARSE_BODY_START = 7,
	/// Continuation of an incomplete body.
	/// Used when packet length is given
	PSYC_PARSE_BODY_CONT = 8,
	/// End of an incomplete body.
	/// Used when packet length is given
	PSYC_PARSE_BODY_END = 9,
	/// Body parsing done in one go, name contains method, value contains body.
	PSYC_PARSE_BODY = 10,
	/// Start of an incomplete content, value contains part of content.
	/// Used when PSYC_PARSE_ROUTING_ONLY is set.
	PSYC_PARSE_CONTENT_START = 7,
	/// Continuation of an incomplete content.
	/// Used when PSYC_PARSE_ROUTING_ONLY is set.
	PSYC_PARSE_CONTENT_CONT = 8,
	/// End of an incomplete content.
	/// Used when PSYC_PARSE_ROUTING_ONLY is set.
	PSYC_PARSE_CONTENT_END = 9,
	/// Content parsing done in one go, value contains the whole content.
	/// Used when PSYC_PARSE_ROUTING_ONLY is set.
	PSYC_PARSE_CONTENT = 10,
	/// Finished parsing packet.
	PSYC_PARSE_COMPLETE = 11,
} psycParseRC;

/**
 * The return value definitions for the list parsing function.
 * @see psyc_parse_list()
 */
typedef enum {
	PSYC_PARSE_LIST_ERROR_DELIM = -4,
	PSYC_PARSE_LIST_ERROR_LEN = -3,
	PSYC_PARSE_LIST_ERROR_TYPE = -2,
	PSYC_PARSE_LIST_ERROR = -1,
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
typedef struct {
	size_t cursor; ///< Current position in buffer.
	size_t startc; ///< Position where the parsing would be resumed.
	psycString buffer; ///< Buffer with data to be parsed.
	uint8_t flags; ///< Flags for the parser, see psycParseFlag.
	psycPart part; ///< Part of the packet being parsed currently.

	size_t routingLength; ///< Length of routing part parsed so far.
	size_t contentParsed; ///< Number of bytes parsed from the content so far.
	size_t contentLength; ///< Expected length of the content.
	psycBool contentLengthFound; ///< Is there a length given for this packet?
	size_t valueParsed; ///< Number of bytes parsed from the value so far.
	size_t valueLength; ///< Expected length of the value.
	psycBool valueLengthFound; ///< Is there a length given for this modifier?
} psycParseState;

/**
 * Struct for keeping list parser state.
 */
typedef struct {
	size_t cursor; ///< Current position in buffer.
	size_t startc; ///< Line start position.
	psycString buffer; ///< Buffer with data to be parsed.
	psycListType type; ///< List type.

	size_t elemParsed; ///< Number of bytes parsed from the elem so far.
	size_t elemLength; ///< Expected length of the elem.
} psycParseListState;

/**
 * Initializes the state struct.
 *
 * @param state Pointer to the state struct that should be initialized.
 * @param flags Flags to be set for the parser, see psycParseFlag.
 * @see psycParseFlag
 */
static inline
void psyc_parse_state_init (psycParseState *state, uint8_t flags)
{
	memset(state, 0, sizeof(psycParseState));
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
 * @see psycString
 */
static inline
void psyc_parse_buffer_set (psycParseState *state, char *buffer, size_t length)
{
	state->buffer = (psycString) {length, buffer};
	state->cursor = 0;

	if (state->flags & PSYC_PARSE_START_AT_CONTENT) {
		state->contentLength = length;
		state->contentLengthFound = PSYC_TRUE;
	}
}

/**
 * Initializes the list state struct.
 *
 * @param state Pointer to the list state struct that should be initialized.
 */
static inline
void psyc_parse_list_state_init (psycParseListState *state)
{
	memset(state, 0, sizeof(psycParseListState));
}

/**
 * Sets a new buffer in the list parser state struct with data to be parsed.
 */
static inline
void psyc_parse_list_buffer_set (psycParseListState *state, char *buffer, size_t length)
{
	state->buffer = (psycString) {length, buffer};
	state->cursor = 0;
}

static inline
size_t psyc_parse_content_length (psycParseState *state)
{
	return state->contentLength;
}

static inline
psycBool psyc_parse_content_length_found (psycParseState *state)
{
	return state->contentLengthFound;
}

static inline
size_t psyc_parse_value_length (psycParseState *state)
{
	return state->valueLength;
}

static inline
psycBool psyc_parse_value_length_found (psycParseState *state)
{
	return state->valueLengthFound;
}

static inline
size_t psyc_parse_cursor (psycParseState *state)
{
	return state->cursor;
}

static inline
size_t psyc_parse_buffer_length (psycParseState *state)
{
	return state->buffer.length;
}

static inline
size_t psyc_parse_remaining_length (psycParseState *state)
{
	return state->buffer.length - state->cursor;
}

static inline
const char * psyc_parse_remaining_buffer (psycParseState *state)
{
	return state->buffer.ptr + state->cursor;
}

/**
 * Parse PSYC packets.
 *
 * This function parses a full or partial PSYC packet while keeping parsing
 * state in a state variable that you have to pass in every time, and returns
 * whenever a modifier or the body is found. See psycParseRC for the possible
 * return codes. When it returns oper, name & value will point to the respective
 * parts of the buffer, no memory allocation is done.
 *
 * @param state An initialized psycParseState.
 * @param oper In case of a modifier it will be set to the operator.
 * @param name In case of a modifier it will point to the name,
 *             in case of the body it will point to the method.
 * @param value In case of a modifier it will point to the value,
 *              in case of the body it will point to the data.
 */
#ifdef __INLINE_PSYC_PARSE
static inline
#endif
psycParseRC psyc_parse (psycParseState *state, char *oper,
                        psycString *name, psycString *value);

/**
 * List parser.
 *
 * This function parses a _list modifier value and returns one element a time
 * while keeping parsing state in a state variable that you have to pass in
 * every time. When it returns elem will point to the next element in value, no
 * memory allocation is done.
 *
 * @param state An initialized psycParseListState.
 * @param elem It will point to the next element in the list.
 */
#ifdef __INLINE_PSYC_PARSE
static inline
#endif
psycParseListRC psyc_parse_list (psycParseListState *state, psycString *elem);

static inline
psycBool psyc_parse_number (const char *value, size_t len, ssize_t *n)
{
	size_t c = 0;
	uint8_t neg = 0;

	if (!value)
		return PSYC_FALSE;

	if (value[0] == '-')
		neg = ++c;

	*n = 0;
	while (c < len && value[c] >= '0' && value[c] <= '9')
		*n = 10 * *n + (value[c++] - '0');

	if (c != len)
		return PSYC_FALSE;

	if (neg)
		*n = 0 - *n;

	return PSYC_TRUE;
}

static inline
psycBool psyc_parse_time (const char *value, size_t len, time_t *t)
{
	return psyc_parse_number(value, len, t);
}

static inline
psycBool psyc_parse_date (const char *value, size_t len, time_t *t)
{
	if (psyc_parse_number(value, len, t)) {
		*t += PSYC_EPOCH;
		return PSYC_TRUE;
	}
	return PSYC_FALSE;
}

/**
 * Determines if the argument is a glyph.
 * Glyphs are: : = + - ? !
 */
static inline
char psyc_is_glyph (uint8_t g)
{
	switch(g) {
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
char psyc_is_numeric (uint8_t c)
{
	return c >= '0' && c <= '9';
}

/**
 * Determines if the argument is alphabetic.
 */
static inline
char psyc_is_alpha (uint8_t c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/**
 * Determines if the argument is alphanumeric.
 */
static inline
char psyc_is_alpha_numeric (uint8_t c)
{
	return psyc_is_alpha(c) || psyc_is_numeric(c);
}

/**
 * Determines if the argument is a keyword character.
 * Keyword characters are: alphanumeric and _
 */
static inline
char psyc_is_kw_char (uint8_t c)
{
	return psyc_is_alpha_numeric(c) || c == '_';
}

/**
 * Determines if the argument is a name character.
 * Name characters are: see opaque_part in RFC 2396
 */
static inline
char psyc_is_name_char (uint8_t c)
{
	return psyc_is_alpha(c) || (c >= '$' && c <= ';') ||
		c == '_' || c == '!' || c == '?' || c == '=' || c == '@' || c == '~';
}

/**
 * Determines if the argument is a hostname character.
 * Hostname characters are: alphanumeric and -
 */
static inline
char psyc_is_host_char (uint8_t c)
{
	return psyc_is_alpha_numeric(c) || c == '.' || c == '-';
}

/** @} */ // end of parse group

#define PSYC_PARSE_H
#endif
