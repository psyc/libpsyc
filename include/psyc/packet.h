#ifndef PSYC_PACKET_H

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
typedef enum
{
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
typedef enum
{
	/// List needs to be checked if it needs length.
	PSYC_LIST_CHECK_LENGTH = 0,
	/// List needs length.
	PSYC_LIST_NEED_LENGTH = 1,
	/// List doesn't need length.
	PSYC_LIST_NO_LENGTH = 2,
} PsycListFlag;

/** Packet flags. */
typedef enum
{
	/// Packet needs to be checked if it needs content length.
	PSYC_PACKET_CHECK_LENGTH = 0,
	/// Packet needs content length.
	PSYC_PACKET_NEED_LENGTH = 1,
	/// Packet doesn't need content length.
	PSYC_PACKET_NO_LENGTH = 2,
} PsycPacketFlag;

/** Structure for a modifier. */
typedef struct
{
	char oper;
	PsycString name;
	PsycString value;
	PsycModifierFlag flag;
} PsycModifier;

/** Structure for an entity or routing header. */
typedef struct
{
	size_t lines;
	PsycModifier *modifiers;
} PsycHeader;

/** Structure for a list. */
typedef struct
{
	size_t num_elems;
	PsycString *elems;
	size_t length;
	PsycListFlag flag;
} PsycList;

/** Intermediate struct for a PSYC packet */
typedef struct
{
	PsycHeader routing;	///< Routing header.
	PsycHeader entity;	///< Entity header.
	PsycString method; ///< Contains the method.
	PsycString data; ///< Contains the data.
	PsycString content; ///< Contains the whole content.
	size_t routingLength;	///< Length of routing part.
	size_t contentLength;	///< Length of content part.
	size_t length;		///< Total length of packet.
	PsycPacketFlag flag; ///< Packet flag.
} PsycPacket;

/**
 * Return the number of digits a number has in its base 10 representation.
 */
static inline
size_t psyc_num_length (size_t n)
{
	return n < 10 ? 1 : log10(n) + 1;
}

/**
 * \internal
 * Check if a modifier needs length.
 */
static inline
PsycModifierFlag psyc_modifier_length_check (PsycModifier *m)
{
	PsycModifierFlag flag;

	if (m->value.length > PSYC_MODIFIER_SIZE_THRESHOLD)
		flag = PSYC_MODIFIER_NEED_LENGTH;
	else if (memchr(m->value.data, (int)'\n', m->value.length))
		flag = PSYC_MODIFIER_NEED_LENGTH;
	else
		flag = PSYC_MODIFIER_NO_LENGTH;

	return flag;
}

/** Create new modifier */
static inline
PsycModifier psyc_modifier_new (char oper,
                                char *name, size_t namelen,
                                char *value, size_t valuelen,
                                PsycModifierFlag flag){

	PsycModifier m = {oper, {namelen, name}, {valuelen, value}, flag};

	if (flag == PSYC_MODIFIER_CHECK_LENGTH) // find out if it needs a length
		m.flag = psyc_modifier_length_check(&m);

	return m;
}

/**
 * \internal
 * Get the total length of a modifier when rendered.
 */
size_t psyc_modifier_length (PsycModifier *m);

/**
 * \internal
 * Check if a list needs length.
 */
PsycListFlag psyc_list_length_check (PsycList *list);

/**
 * \internal
 * Get the total length of a list when rendered.
 */
PsycListFlag psyc_list_length (PsycList *list);

/**
 * \internal
 * Check if a packet needs length.
 */
PsycPacketFlag psyc_packet_length_check (PsycPacket *p);

/**
 * Calculate and set the rendered length of packet parts and total packet length.
 */
size_t psyc_packet_length_set (PsycPacket *p);

/** Create new list. */
PsycList psyc_list_new (PsycString *elems, size_t num_elems, PsycListFlag flag);

/** Create new packet. */
PsycPacket psyc_packet_new (PsycModifier *routing, size_t routinglen,
                            PsycModifier *entity, size_t entitylen,
                            char *method, size_t methodlen,
                            char *data, size_t datalen,
                            PsycPacketFlag flag);

/** Create new packet with raw content. */
PsycPacket psyc_packet_new_raw (PsycModifier *routing, size_t routinglen,
                                char *content, size_t contentlen,
                                PsycPacketFlag flag);

/** @} */ // end of packet group

#define PSYC_PACKET_H
#endif
