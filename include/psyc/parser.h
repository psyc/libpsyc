#include <stdint.h>
#include <string.h>

#ifndef PSYC_PARSER_H_INCLUDED
#define PSYC_PARSER_H_INCLUDED

enum PSYC_Flags
{
	PSYC_HEADER_ONLY = 1
};


enum PSYC_ReturnCodes
{
	PSYC_ERROR_END = -7,
	PSYC_ERROR_METHOD = -6,
	PSYC_ERROR_VAR_LEN = -5,
	PSYC_ERROR_VAR_TAB = -4,
	PSYC_ERROR_VAR_NAME = -3,
	PSYC_ERROR_LENGTH = -2,
	PSYC_ERROR = -1,
	PSYC_SUCCESS = 0,
	PSYC_BODY = 1,
	PSYC_BODY_INCOMPLETE = 2,
	PSYC_INSUFFICIENT = 3,
	PSYC_ROUTING = 4,
	PSYC_ENTITY = 5,
	PSYC_ENTITY_INCOMPLETE = 6,
	PSYC_HEADER_COMPLETE = 7,
	PSYC_COMPLETE = 8,
	PSYC_INCOMPLETE = 9,
};

enum PSYC_ListReturnCodes
{
	PSYC_ERROR_LIST_DELIM = -5,
	PSYC_ERROR_LIST_LEN = -4,
	PSYC_ERROR_LIST_TYPE = -3,
	PSYC_ERROR_LIST_NAME = -2,
	PSYC_ERROR_LIST= -1,
	PSYC_LIST_ELEM = 1,
	PSYC_LIST_END = 2,
	PSYC_LIST_INCOMPLETE = 3,
};

enum PSYC_Parts
{
	PSYC_PART_RESET = -1,
	PSYC_PART_HEADER = 0,
	PSYC_PART_LENGTH,
	PSYC_PART_CONTENT,
	PSYC_PART_METHOD,
	PSYC_PART_DATA,
	PSYC_PART_END,
};

enum PSYC_ListTypes
{
	PSYC_LIST_TEXT = 1,
	PSYC_LIST_BINARY = 2,
};

typedef struct
{
	size_t length;
	const uint8_t * ptr;
} PSYC_Array;


typedef struct
{
	size_t cursor; /** current position in buffer */
	size_t startc; /** line start position */
	PSYC_Array buffer;
	uint8_t flags;
	char part; /** part of the packet being parsed currently, see PSYC_Parts */

	size_t contentParsed; /** number of bytes parsed from the content so far */
	size_t contentLength; /** expected length of the content */
	char contentLengthFound; /** is there a length given for this packet? */
	size_t valueParsed; /** number of bytes parsed from the value so far */
	size_t valueLength; /** expected length of the value */
} PSYC_State;

typedef struct
{
	size_t cursor; /** current position in buffer */
	size_t startc; /** line start position */
	PSYC_Array buffer;
	char type; /** list type, see PSYC_ListTypes */

	size_t elemParsed; /** number of bytes parsed from the elem so far */
	size_t elemLength; /** expected length of the elem */
} PSYC_ListState;

#ifndef PSYC_COMPILE_LIBRARY
/* @brief Shortcut for creating an array.
 *
 * @param memory Pointer to the buffer.
 * @param length Length of that buffer.
 *
 * @return An instance of the PSYC_Array struct. */
inline PSYC_Array PSYC_createArray (uint8_t* const memory, size_t length)
{
	PSYC_Array arr = {length, memory};
	return arr;
}

/* @brief Initiates the state struct with flags.
 *
 * @param state Pointer to the state struct that should be initiated.
 * @param flags The flags that one ones to set, see PSYC_Flags.
 */
inline void PSYC_initState2 (PSYC_State* state, uint8_t flags )
{
	memset(state, 0, sizeof(PSYC_State));
	state->flags = flags;
}

/* @brief Initiates the state struct.
 *
 * @param state Pointer to the state struct that should be initiated.
 */
inline void PSYC_initState (PSYC_State* state)
{
	memset(state, 0, sizeof(PSYC_State));
}

/* @brief Initiates the list state struct.
 *
 * @param state Pointer to the list state struct that should be initiated.
 */
inline void PSYC_initListState (PSYC_ListState* state)
{
	memset(state, 0, sizeof(PSYC_ListState));
}

inline void PSYC_nextBuffer (PSYC_State* state, PSYC_Array newBuf)
{
	state->buffer = newBuf;
	state->cursor = 0;
}

inline void PSYC_nextListBuffer (PSYC_ListState* state, PSYC_Array newBuf)
{
	state->buffer = newBuf;
	state->cursor = 0;
}

inline size_t PSYC_getContentLength (PSYC_State* s)
{
	return s->contentLength;
}

#endif

int PSYC_parse(PSYC_State* state, uint8_t* modifier, PSYC_Array* name, PSYC_Array* value);

int PSYC_parseList(PSYC_ListState* state, PSYC_Array *name, PSYC_Array* value, PSYC_Array* elem);


#endif
