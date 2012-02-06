#ifndef PSYC_PACKET_H
#define PSYC_PACKET_H

/**
 * @file psyc/packet.h
 * @brief Interface for PSYC packet data structures.
 *
 * Packet data structures and functions for creating them are defined here.
 */

/**
 * @defgroup packet Packet data structures
 *
 * This module contains definitions of packet data structures and functions for
 * creating them.
 * @{
 */

#include <math.h>

#include "method.h"

/**
 * Maximum allowed content size without length.
 * Parser stops after reaching this limit.
 */
#define PSYC_CONTENT_SIZE_MAX 512
/**
 * Content size after which a length is added when rendering.
 */
#ifndef PSYC_CONTENT_SIZE_THRESHOLD
# define PSYC_CONTENT_SIZE_THRESHOLD 9
#endif

/**
 * Maximum allowed modifier size without length.
 * Parser stops after reaching this limit.
 */
#define PSYC_MODIFIER_SIZE_MAX 256
/**
 * Modifier size after which a length is added when rendering.
 */
#ifndef PSYC_MODIFIER_SIZE_THRESHOLD
# define PSYC_MODIFIER_SIZE_THRESHOLD 9
#endif

/**
 * Maximum allowed element size without length.
 * Parser stops after reaching this limit.
 */
#define PSYC_ELEM_SIZE_MAX 128
/**
 * Element size after which a length is added when rendering.
 */
#ifndef PSYC_ELEM_SIZE_THRESHOLD
# define PSYC_ELEM_SIZE_THRESHOLD 9
#endif

#define PSYC_PACKET_DELIMITER_CHAR '|'
#define PSYC_PACKET_DELIMITER	   "\n|\n"

#define PSYC_LIST_ELEM_START '|'
#define PSYC_DICT_KEY_START '{'
#define PSYC_DICT_KEY_END '}'
#define PSYC_DICT_VALUE_START PSYC_DICT_KEY_END
#define PSYC_DICT_VALUE_END PSYC_DICT_KEY_START

/** Modifier flags. */
typedef enum {
    /// Modifier needs to be checked if it needs length.
    PSYC_MODIFIER_CHECK_LENGTH = 0,
    /// Modifier needs length.
    PSYC_MODIFIER_NEED_LENGTH = 1,
    /// Modifier doesn't need length.
    PSYC_MODIFIER_NO_LENGTH = 2,
    /// Routing modifier, which implies that it doesn't need length.
    PSYC_MODIFIER_ROUTING = 4,
} PsycModifierFlag;

/** List/dict element flags. */
typedef enum {
    /// Element needs to be checked if it needs length.
    PSYC_ELEM_CHECK_LENGTH = 0,
    /// Element needs length.
    PSYC_ELEM_NEED_LENGTH = 1,
    /// Element doesn't need length.
    PSYC_ELEM_NO_LENGTH = 2,
} PsycElemFlag;

/** Packet flags. */
typedef enum {
    /// Packet needs to be checked if it needs content length.
    PSYC_PACKET_CHECK_LENGTH = 0,
    /// Packet needs content length.
    PSYC_PACKET_NEED_LENGTH = 1,
    /// Packet doesn't need content length.
    PSYC_PACKET_NO_LENGTH = 2,
} PsycPacketFlag;

typedef enum {
    PSYC_OPERATOR_SET = ':',
    PSYC_OPERATOR_ASSIGN = '=',
    PSYC_OPERATOR_AUGMENT = '+',
    PSYC_OPERATOR_DIMINISH = '-',
    PSYC_OPERATOR_UPDATE = '@',
    PSYC_OPERATOR_QUERY = '?',
} PsycOperator;

typedef enum {
    PSYC_STATE_NOOP = 0,
    PSYC_STATE_RESET = '=',
    PSYC_STATE_RESYNC = '?',
} PsycStateOp;

typedef enum {
    PSYC_PACKET_ID_CONTEXT = 0,
    PSYC_PACKET_ID_SOURCE = 1,
    PSYC_PACKET_ID_TARGET = 2,
    PSYC_PACKET_ID_COUNTER = 3,
    PSYC_PACKET_ID_FRAGMENT = 4,
    PSYC_PACKET_ID_ELEMS = 5,
} PsycPacketId;

typedef struct {
    PsycString type;
    PsycString value;
    size_t length;
    PsycElemFlag flag;
} PsycElem;

#define PSYC_ELEM(typ, typlen, val, vallen, flg)	\
    (PsycElem) {					\
	.type = PSYC_STRING(typ, typlen),		\
	.value = PSYC_STRING(val, vallen),		\
	.flag = flg,					\
     }
#define PSYC_ELEM_TV(typ, typlen, val, vallen)		\
    (PsycElem) {					\
	.type = PSYC_STRING(typ, typlen),		\
	.value = PSYC_STRING(val, vallen),		\
     }
#define PSYC_ELEM_VF(val, vallen, flg)			\
    (PsycElem) {					\
	.value = PSYC_STRING(val, vallen),		\
	.flag = flg,					\
    }
#define PSYC_ELEM_V(val, vallen)			\
    (PsycElem) {					\
	.value = PSYC_STRING(val, vallen),		\
    }

/** Dict key */
typedef struct {
    PsycString value;
    size_t length;
    PsycElemFlag flag;
} PsycDictKey;

#define PSYC_DICT_KEY(k, klen, flg)			\
    (PsycDictKey) {					\
	.value = PSYC_STRING(k, klen),			\
	.flag = flg,					\
    }

/** Dict key/value */
typedef struct {
    PsycElem value;
    PsycDictKey key;
} PsycDictElem;

#define PSYC_DICT_ELEM(k, v)				\
    (PsycDictElem) {					\
	.key = k,					\
	.value = v,					\
    }

/** Dictionary */
typedef struct {
    PsycString type;
    PsycDictElem *elems;
    size_t num_elems;
    size_t length;
} PsycDict;

/** List */
typedef struct {
    PsycString type;
    PsycElem *elems;
    size_t num_elems;
    size_t length;
} PsycList;

/** Modifier */
typedef struct {
    PsycString name;
    PsycString value;
    PsycModifierFlag flag;
    char oper;
} PsycModifier;

/** Entity or routing header */
typedef struct {
    size_t lines;
    PsycModifier *modifiers;
} PsycHeader;

/** Intermediate struct for a PSYC packet. */
typedef struct {
    PsycHeader routing;		///< Routing header.
    PsycHeader entity;		///< Entity header.
    PsycString method;		///< Contains the method.
    PsycString data;		///< Contains the data.
    PsycString content;		///< Contains the whole content.
    size_t routinglen;		///< Length of routing part.
    size_t contentlen;		///< Length of content part.
    size_t length;		///< Total length of packet.
    PsycStateOp stateop;	///< State operation. @see PsycStateOp
    PsycPacketFlag flag;	///< Packet flag.
} PsycPacket;

/**
 * Return the number of digits a number has in its base 10 representation.
 */
static inline size_t
psyc_num_length (size_t n)
{
    return n < 10 ? 1 : log10(n) + 1;
}

/**
 * \internal
 * Check if a modifier needs length.
 */
static inline PsycModifierFlag
psyc_modifier_length_check (PsycModifier *m)
{
    if (m->value.length > 0
	&& (m->value.length > PSYC_MODIFIER_SIZE_THRESHOLD
	    || memchr(m->value.data, (int) '\n', m->value.length)))
	return PSYC_MODIFIER_NEED_LENGTH;

    return PSYC_MODIFIER_NO_LENGTH;
}

/** Initialize modifier */
static inline void
psyc_modifier_init (PsycModifier *m, PsycOperator oper,
		    char *name, size_t namelen,
		    char *value, size_t valuelen, PsycModifierFlag flag)
{
    *m = (PsycModifier) {
	.oper = oper,
	.name = {namelen, name},
	.value = {valuelen, value},
	.flag = flag
    };

    if (flag == PSYC_MODIFIER_CHECK_LENGTH) // find out if it needs a length
	m->flag = psyc_modifier_length_check(m);
    else if (flag & PSYC_MODIFIER_ROUTING)
	m->flag |= PSYC_MODIFIER_NO_LENGTH;
}

/**
 * \internal
 * Check if a list/dict element needs length.
 */
PsycElemFlag
psyc_elem_length_check (PsycString *value, const char end);

/**
 * \internal
 * Get the rendered length of a list/dict element.
 */
size_t
psyc_elem_length (PsycElem *elem);

/**
 * \internal
 * Get the rendered length of a dict key.
 */
size_t
psyc_dict_key_length (PsycDictKey *elem);

/**
 * \internal
 * Get the rendered length of a list.
 */
size_t
psyc_list_length_set (PsycList *list);

/**
 * \internal
 * Get the rendered length of a dict.
 */
size_t
psyc_dict_length_set (PsycDict *dict);

/**
 * \internal
 * Get the rendered length of a modifier.
 */
size_t
psyc_modifier_length (PsycModifier *m);

/**
 * \internal
 * Check if a packet needs length.
 */
PsycPacketFlag
psyc_packet_length_check (PsycPacket *p);

/**
 * Calculate and set the rendered length of packet parts and total packet length.
 */
size_t
psyc_packet_length_set (PsycPacket *p);

/** Initialize a list. */
void
psyc_list_init (PsycList *list, PsycElem *elems, size_t num_elems);

/** Initialize a dict. */
void
psyc_dict_init (PsycDict *dict, PsycDictElem *elems, size_t num_elems);

/** Initialize a packet. */
void
psyc_packet_init (PsycPacket *packet,
		  PsycModifier *routing, size_t routinglen,
		  PsycModifier *entity, size_t entitylen,
		  char *method, size_t methodlen,
		  char *data, size_t datalen,
		  char stateop, PsycPacketFlag flag);

/** Initialize packet with raw content. */
void
psyc_packet_init_raw (PsycPacket *packet,
		      PsycModifier *routing, size_t routinglen,
		      char *content, size_t contentlen,
		      PsycPacketFlag flag);

void
psyc_packet_id (PsycList *list, PsycElem *elems,
		char *context, size_t contextlen,
		char *source, size_t sourcelen,
		char *target, size_t targetlen,
		char *counter, size_t counterlen,
		char *fragment, size_t fragmentlen); 

/** @} */ // end of packet group

#endif
