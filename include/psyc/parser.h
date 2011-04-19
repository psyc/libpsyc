#include <stdint.h>
#include <string.h>

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

enum PSYC_Parts
{
	PSYC_PART_RESET = -1,
	PSYC_PART_HEADER = 0,
	PSYC_PART_LENGTH,
	PSYC_PART_HEADER_END,
	PSYC_PART_CONTENT,
	PSYC_PART_METHOD,
	PSYC_PART_DATA,
	PSYC_PART_END,
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
	size_t valueParsed; /** number of bytes parsef from the value so far */
	size_t valueLength; /** expected length of the value */
} PSYC_State;

#ifndef PSYC_COMPILE_LIBRARY
/* @brief shortcut for creating an array 
 *
 * @param memory pointer to the buffer
 * @param length length of that buffer
 *
 * @returns an instance of the PSYC_Array struct */
inline PSYC_Array PSYC_createArray (uint8_t* const memory, size_t length)
{
	PSYC_Array arr = {length, memory};

	return arr;
}

/* @brief initiates the state struct with flags 
 *
 * @param state pointer to the state struct that should be initiated
 * @param flags the flags that one ones to set, see PSYC_Flags */
inline void PSYC_initState2 (PSYC_State* state, uint8_t flags )
{
	memset(state, 0, sizeof(PSYC_State));
	state->flags = flags;
}

/* @brief initiates the state struct 
 *
 * @param state pointer to the state struct that should be initiated */
inline void PSYC_initState (PSYC_State* state)
{
	memset(state, 0, sizeof(PSYC_State));
}

inline void PSYC_nextBuffer (PSYC_State* state, PSYC_Array newBuf)
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
