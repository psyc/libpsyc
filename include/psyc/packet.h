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
} psycModifierFlag;

/** List flags. */
typedef enum
{
	/// List needs to be checked if it needs length.
	PSYC_LIST_CHECK_LENGTH = 0,
	/// List needs length.
	PSYC_LIST_NEED_LENGTH = 1,
	/// List doesn't need length.
	PSYC_LIST_NO_LENGTH = 2,
} psycListFlag;

/** Packet flags. */
typedef enum
{
	/// Packet needs to be checked if it needs content length.
	PSYC_PACKET_CHECK_LENGTH = 0,
	/// Packet needs content length.
	PSYC_PACKET_NEED_LENGTH = 1,
	/// Packet doesn't need content length.
	PSYC_PACKET_NO_LENGTH = 2,
} psycPacketFlag;

/** Structure for a modifier. */
typedef struct
{
	char oper;
	psycString name;
	psycString value;
	psycModifierFlag flag;
} psycModifier;

/** Structure for an entity or routing header. */
typedef struct
{
	size_t lines;
	psycModifier *modifiers;
} psycHeader;

/** Structure for a list. */
typedef struct
{
	size_t num_elems;
	psycString *elems;
	size_t length;
	psycListFlag flag;
} psycList;

/** Intermediate struct for a PSYC packet */
typedef struct
{
	psycHeader routing;	///< Routing header.
	psycHeader entity;	///< Entity header.
	psycString method; ///< Contains the method.
	psycString data; ///< Contains the data.
	psycString content; ///< Contains the whole content.
	size_t routingLength;	///< Length of routing part.
	size_t contentLength;	///< Length of content part.
	size_t length;		///< Total length of packet.
	psycPacketFlag flag; ///< Packet flag.
} psycPacket;

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
psycModifierFlag psyc_modifier_length_check (psycModifier *m)
{
	psycModifierFlag flag;

	if (m->value.length > PSYC_MODIFIER_SIZE_THRESHOLD)
		flag = PSYC_MODIFIER_NEED_LENGTH;
	else if (memchr(m->value.ptr, (int)'\n', m->value.length))
		flag = PSYC_MODIFIER_NEED_LENGTH;
	else
		flag = PSYC_MODIFIER_NO_LENGTH;

	return flag;
}

/** Create new modifier */
static inline
psycModifier psyc_modifier_new (char oper,
                                char *name, size_t namelen,
                                char *value, size_t valuelen,
                                psycModifierFlag flag){

	psycModifier m = {oper, {namelen, name}, {valuelen, value}, flag};

	if (flag == PSYC_MODIFIER_CHECK_LENGTH) // find out if it needs a length
		m.flag = psyc_modifier_length_check(&m);

	return m;
}

/**
 * \internal
 * Get the total length of a modifier when rendered.
 */
size_t psyc_modifier_length (psycModifier *m);

/**
 * \internal
 * Check if a list needs length.
 */
psycListFlag psyc_list_length_check (psycList *list);

/**
 * \internal
 * Get the total length of a list when rendered.
 */
psycListFlag psyc_list_length (psycList *list);

/**
 * \internal
 * Check if a packet needs length.
 */
psycPacketFlag psyc_packet_length_check (psycPacket *p);

/**
 * Calculate and set the rendered length of packet parts and total packet length.
 */
size_t psyc_packet_length_set (psycPacket *p);

/** Create new list. */
psycList psyc_list_new (psycString *elems, size_t num_elems, psycListFlag flag);

/** Create new packet. */
psycPacket psyc_packet_new (psycModifier *routing, size_t routinglen,
                            psycModifier *entity, size_t entitylen,
                            char *method, size_t methodlen,
                            char *data, size_t datalen,
                            psycPacketFlag flag);

/** Create new packet with raw content. */
psycPacket psyc_packet_new_raw (psycModifier *routing, size_t routinglen,
                                char *content, size_t contentlen,
                                psycPacketFlag flag);

/** @} */ // end of packet group

#define PSYC_PACKET_H
#endif
