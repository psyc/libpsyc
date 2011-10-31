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

#define	PSYC_C2STR(str) {sizeof(str)-1, str}
#define	PSYC_C2ARG(str) str, sizeof(str)-1
#define	PSYC_S2ARG(str) (str).ptr, (str).length
#define	PSYC_S2ARG2(str) (str).length, (str).ptr

#define PSYC_NUM_ELEM(a) (sizeof(a) / sizeof(*(a)))

typedef enum
{
	PSYC_FALSE = 0,
	PSYC_TRUE = 1,
	PSYC_NO = 0,
	PSYC_YES = 1,
} PsycBool;

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
} PsycPart;

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
} PsycType;

/**
 * List types.
 * Possible types are text and binary.
 */
typedef enum
{
	PSYC_LIST_TEXT = 1,
	PSYC_LIST_BINARY = 2,
} PsycListType;

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
	char *ptr;
} PsycString;

typedef struct
{
	PsycString key;
	int value;
} PsycMatchVar;

/**
 * Shortcut for creating a PsycString.
 *
 * @param str Pointer to the buffer.
 * @param len Length of that buffer.
 *
 * @return An instance of the PsycString struct.
 */
static inline
PsycString psyc_string_new (char *str, size_t len)
{
	PsycString s = {len, str};
	return s;
}

static inline
unsigned int psyc_version ()
{
	return 1;
}


/**
 * Checks if long keyword string inherits from short keyword string.
 */
int psyc_inherits (char *sho, size_t slen,
                   char *lon, size_t llen);

/**
 * Checks if short keyword string matches long keyword string.
 */
int psyc_matches (char *sho, size_t slen,
                  char *lon, size_t llen);

/**
 * Check if keyword is in array.
 *
 * @param array The array to search, should be ordered alphabetically.
 * @param size Size of array.
 * @param kw Keyword to look for.
 * @param kwlen Length of keyword.
 * @param inherit If true, also look for anything inheriting from kw,
 otherwise only exact matches are returned.
 * @param matching A temporary array used for keeping track of results.
 *                 Should be the same size as the array we're searching.
 *
 * @return The value of the matched variable in the array.
 */

int psyc_in_array (const PsycMatchVar *array, size_t size,
                   const char *kw, size_t kwlen,
                   PsycBool inherit, int8_t *matching);

#include "psyc/variable.h"

#define PSYC_H
#endif
