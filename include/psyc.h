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
 * PSYC packet parts.
 */
typedef enum
{
	PSYC_PART_RESET = -1,
	PSYC_PART_ROUTING,
	PSYC_PART_LENGTH,
	PSYC_PART_CONTENT,
	PSYC_PART_METHOD,
	PSYC_PART_DATA,
	PSYC_PART_END,
} PSYC_Part;

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
} PSYC_Type;

/**
 * List types.
 * Possible types are text and binary.
 */
typedef enum
{
	PSYC_LIST_TEXT = 1,
	PSYC_LIST_BINARY = 2,
} PSYC_ListType;

typedef enum
{
	PSYC_MODIFIER_CHECK_LENGTH = 0,
	PSYC_MODIFIER_NEED_LENGTH = 1,
	PSYC_MODIFIER_NO_LENGTH = 2,
	PSYC_MODIFIER_ROUTING = 3,
} PSYC_ModifierFlag;

typedef enum
{
	PSYC_PACKET_CHECK_LENGTH = 0,
	PSYC_PACKET_NEED_LENGTH = 1,
	PSYC_PACKET_NO_LENGTH = 2,
} PSYC_PacketFlag;

typedef struct
{
	size_t length;
	const char* ptr;
} PSYC_Array;

/* intermediate struct for a PSYC variable modification */
typedef struct
{
	char oper;  // not call it 'operator' as C++ may not like that..?
	PSYC_Array name;
	PSYC_Array value;
	PSYC_ModifierFlag flag;
} PSYC_Modifier;

/* intermediate struct for a PSYC packet */
typedef struct
{
	PSYC_Modifier** routing; // Routing header
	PSYC_Modifier** entity;	// Entitiy header
	PSYC_Array method;
	PSYC_Array data;
	size_t length; /// Length of content part
	PSYC_PacketFlag flag;
} PSYC_Packet;

PSYC_Modifier PSYC_newModifier(char* oper, PSYC_Array* name, PSYC_Array* value, PSYC_ModifierFlag flag);

PSYC_Modifier PSYC_newModifier2(char* oper, char* name, size_t namelen, char* value, size_t valuelen, PSYC_ModifierFlag flag);

PSYC_Packet PSYC_newPacket(PSYC_Modifier** routing, PSYC_Modifier **entity, PSYC_Array* method, PSYC_Array* data, PSYC_PacketFlag flag);

PSYC_Packet PSYC_newPacket2(PSYC_Modifier** routing, PSYC_Modifier **entity, char* method, size_t methodlen, char* data, size_t datalen, PSYC_PacketFlag flag);

/// Routing vars in alphabetical order.
extern const PSYC_Array PSYC_routingVars[];
/// Number of routing vars.
extern const size_t PSYC_routingVarsNum;

/**
 * Get the type of variable name.
 */
PSYC_Bool PSYC_isRoutingVar(const char* name, size_t len);

/**
 * Get the type of variable name.
 */
PSYC_Type PSYC_getVarType(char* name, size_t len);

/**
 * Checks if long keyword string inherits from short keyword string.
 */
int PSYC_inherits(char* sho, size_t slen,
		  char* lon, size_t llen);

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
