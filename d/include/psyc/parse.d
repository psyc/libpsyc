module psyc.parse;

import psyc.common;

extern (C):

enum ParseFlag
{
	ALL = 0,
	ROUTING_ONLY = 1,
	START_AT_CONTENT = 2,
}

/**
 * The return value definitions for the packet parsing function.
 *
 * See_Also:
 * 	parse()
 */
enum ParseRC
{
	/// Error, packet is not ending with a valid delimiter.
	ERROR_END = -8,
	/// Error, expected NL after the method.
	ERROR_METHOD = -7,
	/// Error, expected NL after a modifier.
	ERROR_MOD_NL = -6,
	/// Error, modifier length is not numeric.
	ERROR_MOD_LEN = -5,
	/// Error, expected TAB before modifier value.
	ERROR_MOD_TAB = -4,
	/// Error, modifier name is missing.
	ERROR_MOD_NAME = -3,
	/// Error, expected NL after the content length.
	ERROR_LENGTH = -2,
	/// Error in packet.
	ERROR = -1,
	/// Buffer contains insufficient amount of data.
	/// Fill another buffer and concatenate it with the end of the current buffer,
	/// from the cursor position to the end.
	INSUFFICIENT = 1,
	/// Routing modifier parsing done.
	/// Operator, name & value contains the respective parts.
	ROUTING = 2,
	/// Start of an incomplete entity modifier.
	/// Operator & name are complete, value is incomplete.
	ENTITY_START = 3,
	/// Continuation of an incomplete entity modifier.
	ENTITY_CONT = 4,
	/// End of an incomplete entity modifier.
	ENTITY_END = 5,
	/// Entity modifier parsing done in one go.
	/// Operator, name & value contains the respective parts.
	ENTITY = 6,
	/// Start of an incomplete body.
	/// Name contains method, value contains part of the body.
	BODY_START = 7,
	/// Continuation of an incomplete body.
	BODY_CONT = 8,
	/// End of an incomplete body.
	BODY_END = 9,
	/// Body parsing done in one go, name contains method, value contains body.
	BODY = 10,
	/// Start of an incomplete content, value contains part of content.
	/// Used when ROUTING_ONLY is set.
	CONTENT_START = 7,
	/// Continuation of an incomplete body.
	/// Used when ROUTING_ONLY is set.
	CONTENT_CONT = 8,
	/// End of an incomplete body.
	/// Used when ROUTING_ONLY is set.
	CONTENT_END = 9,
	/// Content parsing done in one go, value contains the whole content.
	/// Used when ROUTING_ONLY is set.
	CONTENT = 10,
	/// Finished parsing packet.
	COMPLETE = 11
}

/**
 * The return value definitions for the list parsing function.
 * See_Also: parseList()
 */
enum ParseListRC
{
	LIST_ERROR_DELIM = -5,
	LIST_ERROR_LEN   = -4,
	LIST_ERROR_TYPE  = -3,
	LIST_ERROR_NAME  = -2,
	LIST_ERROR       = -1,
/// Completed parsing a list element.
	LIST_ELEM        =  1,
/// Reached end of buffer.
	LIST_END         =  2,
/// Binary list is incomplete.
	LIST_INCOMPLETE  =  3,
}

/**
 * Struct for keeping parser state.
 */
struct ParseState
{
	size_t cursor; ///< current position in buffer
	size_t startc; ///< position where the parsing would be resumed
	String buffer; ///< buffer with data to be parsed
	ubyte flags; ///< flags for the parser, see ParseFlag
	Part part; ///< part of the packet being parsed currently

	size_t routingLength; ///< length of routing part parsed so far
	size_t contentParsed; ///< number of bytes parsed from the content so far
	size_t contentLength; ///< expected length of the content
	Bool contentLengthFound; ///< is there a length given for this packet?
	size_t valueParsed; ///< number of bytes parsed from the value so far
	size_t valueLength; ///< expected length of the value
	Bool valueLengthFound; ///< is there a length given for this modifier?

	/**
	 * Initiates the state struct.
	 */
	static ParseState opCall ( )
	{
		ParseState inst;
		return inst;
	}

	/**
	 * Initializes the state struct with flags.
	 *
	 * Params:
	 * 	flags = Flags to be set for the parser, see ParseFlag.
	 */
	static ParseState opCall ( ParseFlag flags )
	{
		ParseState inst;
		inst.flags = flags;

		if (flags & ParseFlag.START_AT_CONTENT)
			inst.part = Part.CONTENT;

		return inst;
	}

	/**
	 * Parse PSYC packets.
	 *
	 * Generalized line-based packet parser.
	 *
	 * Params:
	 * 	oper  = A reference to a character. In case of a variable, it will
	 *          be set to the operator of that variable
	 * 	name  = A reference to a String. It will point to the name of
	 *          the variable or method and its length will be set accordingly
	 * 	value = A reference to a String. It will point to the
	 *          value/body the variable/method and its length will be set accordingly
	 */
	ParseRC parse ( ref char oper, ref char[] name, ref ubyte[] value )
	{
		return psyc_parse(this, &oper, cast(String*) &name, cast(String*) &value);
	}
	
	/**
	 * Parse PSYC packets.
	 *
	 * Generalized line-based packet parser.
	 *
	 * Params:
	 * 	oper  = A reference to a character. In case of a variable, it will
	 *          be set to the operator of that variable
	 * 	name  = A reference to a String. It will point to the name of
	 *          the variable or method and its length will be set accordingly
	 * 	value = A reference to a String. It will point to the
	 *          value/body the variable/method and its length will be set accordingly
	 */
	ParseRC parse ( ref char oper, ref char[] name, ref char[] value )
	{
		return psyc_parse(this, &oper, cast(String*) &name, cast(String*) &value);
	}

	/**
	 * Sets a new buffer in the parser state struct with data to be parsed.
	 *
	 * Params:
	 * 	buffer the buffer that should be parsed now
	 * 
	 * See_Also: String
	 */
	void setParseBuffer ( String buffer )
	{
		this.buffer = buffer;
		this.cursor = 0;

		if (this.flags & ParseFlag.START_AT_CONTENT)
		{
			this.contentLength      = buffer.length;
			this.contentLengthFound = TRUE;
		}
	}

	/**
	 * Sets a new buffer in the parser state struct with data to be parsed.
	 *
	 * Params:
	 * 	buffer = the buffer that should be parsed now
	 * 
	 * See_Also: String
	 */
	void setParseBuffer ( char[] buffer )
	{
		this.setParseBuffer(*(cast(String*) &buffer));
	}

	size_t getContentLength ( )
	{
		return this.contentLength;
	}

	bool isContentLengthFound ( )
	{
		return this.contentLengthFound == TRUE;
	}

	size_t getValueLength ( )
	{
		return this.valueLength;
	}

	bool isValueLengthFound ( )
	{
		return this.valueLengthFound == true;
	}

	size_t getCursor ( )
	{
		return this.cursor;
	}

	size_t getBufferLength ( )
	{
		return this.buffer.length;
	}

	size_t getRemainingLength ( )
	{
		return this.buffer.length - this.cursor;
	}

	ubyte* getRemainingBuffer ( )
	{
		return cast(ubyte*)this.buffer.ptr + this.cursor;
	}

	void getRemainingBuffer ( ref ubyte[] buf )
	{

		buf = cast(ubyte[])this.buffer.ptr[cursor .. cursor + getRemainingLength()];
	}

}

/**
 * Struct for keeping list parser state.
 */
struct ParseListState
{
	size_t cursor; ///< current position in buffer
	size_t startc; ///< line start position
	String buffer;
	ListType type; ///< list type

	size_t elemParsed; ///< number of bytes parsed from the elem so far
	size_t elemLength; ///< expected length of the elem

	/**
	 * Sets a new buffer with data to be parsed
	 *
	 * Params:
	 * 	buffer = the buffer to be parsed
	 */
	void setBuffer ( String buffer )
	{
		this.buffer = buffer;
		this.cursor = 0;
	}

	/**
	 * Sets a new buffer with data to be parsed
	 *
	 * Params:
	 * 	buffer = the buffer to be parsed
	 */
	void setBuffer ( char[] buffer )
	{
		this.setBuffer(*(cast(String*) &buffer));
	}
}

private:

/**
 * Parse PSYC packets.
 *
 * Generalized line-based packet parser.
 *
 * Params:
 * 	state = An initialized ParseState
 * 	oper  = A pointer to a character. In case of a variable, it will
 *          be set to the operator of that variable
 * 	name  = A pointer to a String. It will point to the name of
 *          the variable or method and its length will be set accordingly
 * 	value = A pointer to a String. It will point to the
 *          value/body the variable/method and its length will be set accordingly
 */
ParseRC psyc_parse(ParseState* state, char* oper, String* name, String* value);

/**
 * List value parser.
 */
ParseListRC psyc_parseList(ParseListState* state, String *name, String* value, String* elem);

/** @} */ // end of parsing group
