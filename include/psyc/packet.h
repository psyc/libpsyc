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

#include <psyc.h>
#include <psyc/syntax.h>
#include <math.h>

/** Modifier flags. */
typedef enum {
    /// Modifier needs to be checked if it needs length.
    PSYC_MODIFIER_CHECK_LENGTH = 0,
    /// Modifier needs length.
    PSYC_MODIFIER_NEED_LENGTH = 1,
    /// Modifier doesn't need length.
    PSYC_MODIFIER_NO_LENGTH = 2,
    /// Routing modifier, which implies that it doesn't need length.
    PSYC_MODIFIER_ROUTING = 3,
} PsycModifierFlag;

/** List flags. */
typedef enum {
    /// List needs to be checked if it needs length.
    PSYC_LIST_CHECK_LENGTH = 0,
    /// List needs length.
    PSYC_LIST_NEED_LENGTH = 1,
    /// List doesn't need length.
    PSYC_LIST_NO_LENGTH = 2,
} PsycListFlag;

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

/** Structure for a modifier. */
typedef struct {
    char oper;
    PsycString name;
    PsycString value;
    PsycModifierFlag flag;
} PsycModifier;

/** Structure for an entity or routing header. */
typedef struct {
    size_t lines;
    PsycModifier *modifiers;
} PsycHeader;

/** Structure for a list. */
typedef struct {
    size_t num_elems;
    PsycString *elems;
    size_t length;
    PsycListFlag flag;
} PsycList;

typedef struct {
    PsycList *list;
    size_t width;
    size_t length;
} PsycTable;

/** Intermediate struct for a PSYC packet */
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
    PsycModifierFlag flag;

    if (m->value.length > PSYC_MODIFIER_SIZE_THRESHOLD)
	flag = PSYC_MODIFIER_NEED_LENGTH;
    else if (memchr(m->value.data, (int) '\n', m->value.length))
	flag = PSYC_MODIFIER_NEED_LENGTH;
    else
	flag = PSYC_MODIFIER_NO_LENGTH;

    return flag;
}

/** Initialize modifier */
static inline void
psyc_modifier_init (PsycModifier *m, char oper,
		    char *name, size_t namelen,
		    char *value, size_t valuelen, PsycModifierFlag flag)
{
    *m = (PsycModifier) {oper, {namelen, name}, {valuelen, value}, flag};

    if (flag == PSYC_MODIFIER_CHECK_LENGTH) // find out if it needs a length
	m->flag = psyc_modifier_length_check(m);
}

/**
 * \internal
 * Get the total length of a modifier when rendered.
 */
size_t
psyc_modifier_length (PsycModifier *m);

/**
 * \internal
 * Check if a list needs length.
 */
PsycListFlag
psyc_list_length_check (PsycList *list);

/**
 * \internal
 * Get the total length of a list when rendered.
 */
PsycListFlag
psyc_list_length (PsycList *list);

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
psyc_list_init (PsycList *list, PsycString *elems, size_t num_elems,
		PsycListFlag flag);

/** Initialize a table. */
void
psyc_table_init (PsycTable *table, size_t width, PsycList *list);

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

/** @} */ // end of packet group

#endif
