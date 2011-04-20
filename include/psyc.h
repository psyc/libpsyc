/** @file psyc.h
 *
 * @brief Main PSYC interface providing crucial functionality.
*/

/** @mainpage PSYC Core Library
 *
 * @section intro_sec Introduction
 *
 * This is the introduction.
 *
 * @section install_sec Installation
 *
 * @subsection step1 Step 1: Opening the box
 *
 * etc...
 */

#ifndef PSYC_H
# define PSYC_H

#include <stdint.h>
#include <string.h>

#define PSYC_EPOCH      1440444041      // 2015-08-24 21:20:41 CET (Monday)

typedef enum
{
	PSYC_FALSE = 0,
	PSYC_TRUE = 1,
} PSYC_Bool;

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
} PSYC_Type;

/**
 * Get the type of variable name.
 */
PSYC_Type PSYC_getVarType(char* name, size_t len);

/**
 * Get the type of variable name.
 */
PSYC_Bool PSYC_isRoutingVar(char* name, size_t len);


/**
 * Checks if short keyword string matches long keyword string.
 */
int PSYC_matches(char* sho, size_t slen,
		 char* lon, size_t llen);

/**
 * Callback for PSYC_text() that produces a value for a match.
 *
 * The application looks up a match such as _fruit from [_fruit] and
 * if found writes its current value from its variable store into the
 * outgoing buffer.. "Apple" for example. The template returns the
 * number of bytes written. 0 is a legal return value. Should the
 * callback return -1, PSYC_text leaves the original template text as is.
 */
typedef int (*PSYC_textCB)(uint8_t* match, size_t  mlen,
		           uint8_t** buffer, size_t * blen);

/**
 * Fills out text templates by asking a callback for content.
 *
 * Copies the contents of the template into the buffer while looking
 * for braceOpen and braceClose strings and calling the callback for
 * each enclosed string between these braces. Should the callback
 * return -1, the original template text is copied as is.
 *
 * By default PSYC's "[" and "]" are used but you can provide any other
 * brace strings such as "${" and "}" or "<!--" and "-->".
 *
 * See also http://about.psyc.eu/psyctext
 */
int PSYC_text(uint8_t* template, size_t  tlen,
	      uint8_t** buffer, size_t * blen,
	      PSYC_textCB lookupValue,
	      char* braceOpen, char* braceClose);

#endif // PSYC_H
