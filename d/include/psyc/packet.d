 /*
 * Packet data structures and functions for creating them are defined here.
 */

module psyc.packet;

import psyc.common;
import psyc.syntax;


extern (C):

/** Modifier flags. */
enum ModifierFlag
{
	/// Modifier needs to be checked if it needs length.
	CHECK_LENGTH = 0,
	/// Modifier needs length.
	NEED_LENGTH = 1,
	/// Modifier doesn't need length.
	NO_LENGTH = 2,
	/// Routing modifier, which implies that it doesn't need length.
	ROUTING = 3,
};

/** List flags. */
enum ListFlag
{
	/// List needs to be checked if it needs length.
	CHECK_LENGTH = 0,
	/// List needs length.
	NEED_LENGTH = 1,
	/// List doesn't need length.
	NO_LENGTH = 2,
} ;

/** Packet flags. */
enum PacketFlag
{
	/// Packet needs to be checked if it needs content length.
	CHECK_LENGTH = 0,
	/// Packet needs content length.
	NEED_LENGTH = 1,
	/// Packet doesn't need content length.
	NO_LENGTH = 2,
} ;

/** Structure for a modifier. */
struct Modifier
{
	char operator;
	String name;
	String value;
	ModifierFlag flag;
	
	static Modifier opCall ( char op, char[] nam, char[] val )
	{
		Modifier v;

		with (v) 
		{
			operator = op;
			name     = nam;
			value    = val;
		}

		return v;
	}
	
	bool opEquals ( ref Modifier n )
	{
		return operator == n.operator &&
		       value == n.value &&
		       name  == n.name;
	}

	Modifier dup ( )
	{
		auto v = M(operator, name.dup, value.dup);
		return v;
	}
};

alias Modifier M;

/** Structure for an entity or routing header. */
struct Header
{
	size_t lines;
	Modifier *modifiers;
} ;

/** Structure for a list. */
struct List
{
	size_t num_elems;
	String *elems;
	size_t length;
	ListFlag flag;
} ;

/** intermediate struct for a PSYC packet */
struct Packet
{
	Header routing;	///< Routing header.
	Header entity;	///< Entity header.
	String method; ///< Contains the method.
	String data; ///< Contains the data.
	String content; ///< Contains the whole content.
	size_t routingLength;	///< Length of routing part.
	size_t contentLength;	///< Length of content part.
	size_t length;		///< Total length of packet.
	PacketFlag flag; ///< Packet flag.
} ;

/**
 * \internal
 * Check if a modifier needs length.
 */

ModifierFlag psyc_checkModifierLength (Modifier *m)
{
	ModifierFlag flag;

	if (m->value.length > PSYC_MODIFIER_SIZE_THRESHOLD)
		flag = ModifierFlag.NEED_LENGTH;
	else if (memchr(m->value.ptr, (int)'\n', m->value.length))
		flag = ModifierFlag.NEED_LENGTH;
	else
		flag = ModifierFlag.NO_LENGTH;

	return flag;
}

/** Create new modifier. */

Modifier psyc_newModifier (char oper, String *name, String *value,
                               ModifierFlag flag)
{
	Modifier m = {oper, *name, *value, flag};

	if (flag == ModifierFlag.CHECK_LENGTH) // find out if it needs a length
		m.flag = psyc_checkModifierLength(&m);

	return m;
}

/** Create new modifier */
Modifier psyc_newModifier2 (char oper,
                                char *name, size_t namelen,
                                char *value, size_t valuelen,
                                ModifierFlag flag)
{
	String n = {namelen, name};
	String v = {valuelen, value};

	return psyc_newModifier(oper, &n, &v, flag);
}

/**
 * \internal
 * Get the total length of a modifier when rendered.
 */
size_t psyc_getModifierLength (Modifier *m);

/**
 * \internal
 * Check if a list needs length.
 */
ListFlag psyc_checkListLength (List *list);

/**
 * \internal
 * Get the total length of a list when rendered.
 */
ListFlag psyc_getListLength (List *list);

/**
 * \internal
 * Check if a packet needs length.
 */
PacketFlag psyc_checkPacketLength (Packet *p);

/**
 * Calculate and set the rendered length of packet parts and total packet length.
 */
size_t psyc_setPacketLength (Packet *p);

/** Create new list. */
List psyc_newList (String *elems, size_t num_elems, ListFlag flag);

/** Create new packet. */
Packet psyc_newPacket (Header *routing,
                           Header *entity,
                           String *method, String *data,
                           PacketFlag flag);

/** Create new packet. */
Packet psyc_newPacket2 (Modifier *routing, size_t routinglen,
                            Modifier *entity, size_t entitylen,
                            char *method, size_t methodlen,
                            char *data, size_t datalen,
                            PacketFlag flag);

/** Create new packet with raw content. */
Packet psyc_newRawPacket (Header *routing, String *content,
                              PacketFlag flag);

/** Create new packet with raw content. */
Packet psyc_newRawPacket2 (Modifier *routing, size_t routinglen,
                               char *content, size_t contentlen,
                               PacketFlag flag);

