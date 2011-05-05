#ifndef PSYC_PACKET_H
# define PSYC_PACKET_H

#include <psyc.h>
#include <psyc/syntax.h>

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

/** intermediate struct for a PSYC variable modification */
typedef struct
{
	char oper;
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

/** intermediate struct for a PSYC packet */
typedef struct
{
	psycHeader routing;	///< Routing header.
	psycHeader entity;	///< Entity header.
	psycString method;
	psycString data;
	psycString content;
	size_t routingLength;	///< Length of routing part.
	size_t contentLength;	///< Length of content part.
	size_t length;		///< Total length of packet.
	psycPacketFlag flag;
} psycPacket;

/** Check if a modifier needs length */
static inline
psycModifierFlag psyc_checkModifierLength (psycModifier *m)
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
psycModifier psyc_newModifier (char oper, psycString *name, psycString *value,
                               psycModifierFlag flag)
{
	psycModifier m = {oper, *name, *value, flag};

	if (flag == PSYC_MODIFIER_CHECK_LENGTH) // find out if it needs a length
		m.flag = psyc_checkModifierLength(&m);

	return m;
}

/** Create new modifier */
static inline
psycModifier psyc_newModifier2 (char oper,
                                const char *name, size_t namelen,
                                const char *value, size_t valuelen,
                                psycModifierFlag flag)
{
	psycString n = {namelen, name};
	psycString v = {valuelen, value};

	return psyc_newModifier(oper, &n, &v, flag);
}

/** Get the total length of a modifier. */
size_t psyc_getModifierLength (psycModifier *m);

/** Check if a list needs length */
psycListFlag psyc_checkListLength (psycList *list);

/** Get the total length of a list. */
psycListFlag psyc_getListLength (psycList *list);

/** Check if a packet needs length */
psycPacketFlag psyc_checkPacketLength (psycPacket *p);

/** Calculate and set the length of packet parts and total packet length  */
size_t psyc_setPacketLength (psycPacket *p);

/** Create new list */
psycList psyc_newList (psycString *elems, size_t num_elems, psycListFlag flag);

/** Create new packet */
psycPacket psyc_newPacket (psycHeader *routing,
                           psycHeader *entity,
                           psycString *method, psycString *data,
                           psycPacketFlag flag);

/** Create new packet */
psycPacket psyc_newPacket2 (psycModifier *routing, size_t routinglen,
                            psycModifier *entity, size_t entitylen,
                            const char *method, size_t methodlen,
                            const char *data, size_t datalen,
                            psycPacketFlag flag);

psycPacket psyc_newRawPacket (psycHeader *routing, psycString *content,
                              psycPacketFlag flag);

psycPacket psyc_newRawPacket2 (psycModifier *routing, size_t routinglen,
                               const char *content, size_t contentlen,
                               psycPacketFlag flag);

#endif // PSYC_PACKET_H
