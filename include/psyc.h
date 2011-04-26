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
} psycBool;

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

typedef enum
{
	PSYC_MODIFIER_CHECK_LENGTH = 0,
	PSYC_MODIFIER_NEED_LENGTH = 1,
	PSYC_MODIFIER_NO_LENGTH = 2,
	PSYC_MODIFIER_ROUTING = 3,
} psycModifierFlag;

typedef enum
{
	PSYC_LIST_CHECK_LENGTH = 0,
	PSYC_LIST_NEED_LENGTH = 1,
	PSYC_LIST_NO_LENGTH = 2,
} psycListFlag;

typedef enum
{
	PSYC_PACKET_CHECK_LENGTH = 0,
	PSYC_PACKET_NEED_LENGTH = 1,
	PSYC_PACKET_NO_LENGTH = 2,
} psycPacketFlag;

typedef struct
{
	size_t length;
	const char *ptr;
} psycString;

/**
 * Shortcut for creating a psycString.
 *
 * @param memory Pointer to the buffer.
 * @param length Length of that buffer.
 *
 * @return An instance of the psycString struct.
 */
inline psycString psyc_newString (const char *str, size_t strlen);

#define	PSYC_C2STR(string) {sizeof(string)-1, string}
#define	PSYC_C2ARG(string) string, sizeof(string)-1

/* intermediate struct for a PSYC variable modification */
typedef struct
{
	char oper;  // not call it 'operator' as C++ may not like that..
	psycString name;
	psycString value;
	psycModifierFlag flag;
} psycModifier;

typedef struct
{
	size_t lines;
	psycModifier *modifiers;
} psycHeader;

typedef struct
{
	size_t num_elems;
	psycString *elems;
	size_t length;
	psycListFlag flag;
} psycList;

/* intermediate struct for a PSYC packet */
typedef struct
{
	psycHeader routing; ///< Routing header.
	psycHeader entity;  ///< Entitiy header.
	psycString method;
	psycString data;
	size_t routingLength; ///< Length of routing part.
	size_t contentLength; ///< Length of content part.
	size_t length; ///< Total length of packet.
	psycPacketFlag flag;
} psycPacket;

inline int psyc_version();

/** Check if a modifier needs length */
inline psycModifierFlag psyc_checkModifierLength(psycModifier *m);

/** Get the total length of a modifier. */
inline size_t psyc_getModifierLength(psycModifier *m);

/** Create new modifier */
inline psycModifier psyc_newModifier(char oper, psycString *name, psycString *value,
                                     psycModifierFlag flag);

/** Create new modifier */
inline psycModifier psyc_newModifier2(char oper,
                                      const char *name, size_t namelen,
                                      const char *value, size_t valuelen,
                                      psycModifierFlag flag);

/** Check if a list needs length */
inline psycListFlag psyc_checkListLength(psycList *list);

/** Get the total length of a list. */
inline psycListFlag psyc_getListLength(psycList *list);

/** Check if a packet needs length */
inline psycPacketFlag psyc_checkPacketLength(psycPacket *p);

/** Calculate and set the length of packet parts and total packet length  */
inline size_t psyc_setPacketLength(psycPacket *p);

/** Create new list */
inline psycList psyc_newList(psycString *elems, size_t num_elems, psycListFlag flag);

/** Create new packet */
inline psycPacket psyc_newPacket(psycHeader *routing,
                                 psycHeader *entity,
                                 psycString *method, psycString *data,
                                 psycPacketFlag flag);

/** Create new packet */
inline psycPacket psyc_newPacket2(psycModifier *routing, size_t routinglen,
                                  psycModifier *entity, size_t entitylen,
                                  const char *method, size_t methodlen,
                                  const char *data, size_t datalen,
                                  psycPacketFlag flag);

/// Routing vars in alphabetical order.
extern const psycString PSYC_routingVars[];
/// Number of routing vars.
extern const size_t PSYC_routingVarsNum;

/**
 * Get the type of variable name.
 */
psycBool psyc_isRoutingVar(const char *name, size_t len);

/**
 * Get the type of variable name.
 */
psycType psyc_getVarType(char *name, size_t len);

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

/**
 * Callback for psyc_text() that produces a value for a match.
 *
 * The application looks up a match such as _fruit from [_fruit] and
 * if found writes its current value from its variable store into the
 * outgoing buffer.. "Apple" for example. The template returns the
 * number of bytes written. 0 is a legal return value. Should the
 * callback return -1, psyc_text leaves the original template text as is.
 */
typedef int (*psyctextCB)(uint8_t *match, size_t  mlen,
                           uint8_t **buffer, size_t *blen);

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
int psyc_text(uint8_t *template, size_t  tlen,
              uint8_t **buffer, size_t *blen,
              psyctextCB lookupValue,
              char *braceOpen, char *braceClose);

#endif // PSYC_H
