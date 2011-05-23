/**
 * @file psyc.h
 * @brief Main PSYC interface providing crucial functionality.
 */

/**
 * @mainpage PSYC Core Library
 *
 * @section intro_sec Introduction
 *
 * Welcome to the libpsyc documentation!
 *
 * @section install_sec Installation
 */
// * @subsection step1 Step 1: Opening the box

#ifndef PSYC_H

#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#define PSYC_EPOCH      1440444041      // 2015-08-24 21:20:41 CET (Monday)

#define	PSYC_C2STR(string) {sizeof(string)-1, string}
#define	PSYC_C2ARG(string) string, sizeof(string)-1

typedef enum
{
	PSYC_FALSE = 0,
	PSYC_TRUE = 1,
} psycBool;

/**
 * PSYC packet parts.
 */
typedef enum
{
	PSYC_PART_RESET = -1,
	PSYC_PART_ROUTING = 0,
	PSYC_PART_LENGTH = 1,
	PSYC_PART_CONTENT = 2,
	PSYC_PART_METHOD = 3,
	PSYC_PART_DATA = 4,
	PSYC_PART_END = 5,
} psycPart;

/**
 * Different types that a variable can have.
 *
 * This enum lists PSYC variable types that
 * this library is capable of checking for
 * validity. Other variable types are treated
 * as opaque data.
 */
typedef enum
{
	PSYC_TYPE_UNKNOWN,
	PSYC_TYPE_AMOUNT,
	PSYC_TYPE_COLOR,
	PSYC_TYPE_DATE,
	PSYC_TYPE_DEGREE,
	PSYC_TYPE_ENTITY,
	PSYC_TYPE_FLAG,
	PSYC_TYPE_LANGUAGE,
	PSYC_TYPE_LIST,
	PSYC_TYPE_NICK,
	PSYC_TYPE_PAGE,
	PSYC_TYPE_UNIFORM,
	PSYC_TYPE_TIME,
} psycType;

/**
 * List types.
 * Possible types are text and binary.
 */
typedef enum
{
	PSYC_LIST_TEXT = 1,
	PSYC_LIST_BINARY = 2,
} psycListType;

/**
 * String struct.
 *
 * Contains pointer and length for a buffer.
 */
typedef struct
{
	/// Length of the data pointed to by ptr
	size_t length; 	
	/// pointer to the data
	const char *ptr;  
} psycString;

typedef struct
{
	psycString name;
	int value;
} psycMatchVar;

/**
 * Shortcut for creating a psycString.
 *
 * @param memory Pointer to the buffer.
 * @param length Length of that buffer.
 *
 * @return An instance of the psycString struct.
 */
static inline
psycString psyc_newString (const char *str, size_t slen)
{
	psycString s = {slen, str};
	return s;
}

static inline
unsigned int psyc_version ()
{
	return 1;
}

/// Routing variables in alphabetical order.
extern const psycString psyc_routingVars[];

// Variable types in alphabetical order.
extern const psycMatchVar psyc_varTypes[];

extern const size_t psyc_routingVarsNum;
extern const size_t psyc_varTypesNum;

/**
 * Is this a routing variable name?
 */
psycBool psyc_isRoutingVar(psycString *name);
/**
 * Is this a routing variable name?
 */
psycBool psyc_isRoutingVar2(const char *name, size_t len);

/**
 * Get the type of variable name.
 */
psycType psyc_getVarType(psycString *name);
/**
 * Get the type of variable name.
 */
psycType psyc_getVarType2(const char *name, size_t len);

/**
 * Is this a list variable name?
 */
static inline
psycBool psyc_isListVar2(const char *name, size_t len)
{
	return len < 5 || memcmp(name, "_list", 5) != 0 ||
		(len > 5 && name[5] != '_') ? PSYC_FALSE : PSYC_TRUE;
}

/**
 * Is this a list variable name?
 */
static inline
psycBool psyc_isListVar(psycString *name)
{
	return psyc_isListVar2(name->ptr, name->length);
}

/**
 * Checks if long keyword string inherits from short keyword string.
 */
int psyc_inherits(char *sho, size_t slen,
                  char *lon, size_t llen);

/**
 * Checks if short keyword string matches long keyword string.
 */
int psyc_matches(char *sho, size_t slen,
                 char *lon, size_t llen);

#define PSYC_H
#endif
