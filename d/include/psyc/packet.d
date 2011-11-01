 /*
 * Packet data structures and functions for creating them are defined here.
 */

module psyc.packet;

import psyc.common;
import psyc.syntax;
import psyc.render;

import tango.stdc.string : memchr;
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
	char[] name;
	String value;
	ModifierFlag flag;
	
	static Modifier opCall ( char op, char[] nam, char[] val, 
	                         ModifierFlag flg = ModifierFlag.CHECK_LENGTH )
	{
		return opCall(op, nam, cast(ubyte[])val, flg);
	}
	static Modifier opCall ( char op, char[] nam, ubyte[] val, 
	                         ModifierFlag flg = ModifierFlag.CHECK_LENGTH )
	{
		Modifier v;

		with (v) 
		{
			operator = op;
			name     = nam;
			value    = val;
			flag     = (flg == ModifierFlag.CHECK_LENGTH) ?
			           checkLength(val) : flg;
		}

		return v;
	}
	
	char[] valuestr ( )
	{
		return cast(char[]) value;
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
	
	size_t length ( )
	{
		return psyc_modifier_length (this);
	}

	private ModifierFlag checkLength ( ubyte[] value )
	{
		ModifierFlag flag;

		if (value.length > PSYC_MODIFIER_SIZE_THRESHOLD)
			flag = ModifierFlag.NEED_LENGTH;
		else if (memchr(value.data, cast(int)'\n', value.length))
			flag = ModifierFlag.NEED_LENGTH;
		else
			flag = ModifierFlag.NO_LENGTH;

		return flag;
	}
};

alias Modifier M;

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
	Modifier[] routing;	///< Routing header.
	Modifier[] entity;	///< Entity header.
	String method; ///< Contains the method.
	String data; ///< Contains the data.
	String content; ///< Contains the whole content.
	size_t routingLength;	///< Length of routing part.
	size_t contentLength;	///< Length of content part.
	size_t _length;		///< Total length of packet.
	PacketFlag flag; ///< Packet flag.
	
	static Packet opCall (Modifier[] routing, Modifier[] entity,
                        char[] method, ubyte[] data,
                        PacketFlag flag = PacketFlag.CHECK_LENGTH)
	{
		return psyc_packet_new(&routing, &entity, cast(ubyte[]*)&method, &data, flag); // FIXME
	}

	static Packet opCall (Modifier[] routing, ubyte[] content,
	                      PacketFlag flag = PacketFlag.CHECK_LENGTH)
	{
		return psyc_packet_new_raw(&routing, &content, flag); // FIXME
	}

	size_t length ( )
	{
		psyc_packet_length_set(this);
		return this._length;
	}

	ubyte[] render ( ubyte[] buffer )
	{
		psyc_packet_length_set(this);

		with (RenderRC) 
			switch (psyc_render(this, buffer.data, buffer.length))
			{
				case ERROR_METHOD_MISSING:
					throw new Exception("Method missing");
					break;
					
				case ERROR_MODIFIER_NAME_MISSING:
					throw new Exception("Modifier name missing");
					break;
					
				case ERROR:
					throw new Exception("Buffer to small");
					break;
					
				case SUCCESS:
					return buffer[0 .. this._length];

				default:
					throw new Exception("Unknown Return Code");
			}
	}
};


/**
 * \internal
 */
private size_t psyc_modifier_length (Modifier *m);

/**
 * \internal
 * Check if a list needs length.
 */
ListFlag psyc_list_length_check (List *list);

/**
 * \internal
 * Get the total length of a list when rendered.
 */
ListFlag psyc_list_length (List *list);

/**
 * \internal
 * Check if a packet needs length.
 */
PacketFlag psyc_packet_length_check (Packet *p);

/**
 * Calculate and set the rendered length of packet parts and total packet length.
 */
private size_t psyc_packet_length_set (Packet *p);

/** Create new list. */
List psyc_list_new (String *elems, size_t num_elems, ListFlag flag);

/** Create new packet. */
Packet psyc_packet_new (Modifier *routing, size_t routinglen,
                        Modifier *entity, size_t entitylen,
                        char *method, size_t methodlen,
                        char *data, size_t datalen,
                        PacketFlag flag);

/** Create new packet with raw content. */
Packet psyc_packet_new_raw (Modifier *routing, size_t routinglen,
                            char *content, size_t contentlen,
                            PacketFlag flag);

