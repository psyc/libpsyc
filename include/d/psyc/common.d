/** @file d/.d
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

module psyc.common;

const EPOCH = 1440444041;      // 2015-08-24 21:20:41 CET (Monday)

extern (C):

enum Bool
{
	FALSE = 0,
	TRUE = 1,
}

/**
 * PSYC packet parts.
 */
enum Part
{
	PART_RESET = -1,
	PART_ROUTING,
	PART_LENGTH,
	PART_CONTENT,
	PART_METHOD,
	PART_DATA,
	PART_END,
}

/**
 * Different types that a variable can have.
 *
 * This enum lists PSYC variable types that
 * this library is capable of checking for
 * validity. Other variable types are treated
 * as opaque data.
 */
enum Type
{
	TYPE_UNKNOWN,
	TYPE_AMOUNT,
	TYPE_COLOR,
	TYPE_DATE,
	TYPE_DEGREE,
	TYPE_ENTITY,
	TYPE_FLAG,
	TYPE_LANGUAGE,
	TYPE_LIST,
	TYPE_NICK,
	TYPE_PAGE,
	TYPE_UNIFORM,
	TYPE_TIME,
}

/**
 * List types.
 * Possible types are text and binary.
 */
enum ListType
{
	LIST_TEXT = 1,
	LIST_BINARY = 2,
} 

enum ModifierFlag
{
	MODIFIER_CHECK_LENGTH = 0,
	MODIFIER_NEED_LENGTH = 1,
	MODIFIER_NO_LENGTH = 2,
	MODIFIER_ROUTING = 3,
}

enum ListFlag
{
	LIST_CHECK_LENGTH = 0,
	LIST_NEED_LENGTH = 1,
	LIST_NO_LENGTH = 2,
}

enum PacketFlag
{
	PACKET_CHECK_LENGTH = 0,
	PACKET_NEED_LENGTH = 1,
	PACKET_NO_LENGTH = 2,
}

struct String
{
	size_t length;
	char *ptr;
}

struct MatchVar
{
	String name;
	int value;
}

/**
* Shortcut for creating a String.
 *
 * @param memory Pointer to the buffer.
 * @param length Length of that buffer.
 *
 * @return An instance of the String struct.
 */
String newString (char *str, size_t strlen);

/* intermediate struct for a PSYC variable modification */
struct Modifier
{
	char oper;  // not call it 'operator' as C++ may not like that..
	String name;
	String value;
	ModifierFlag flag;
}

struct Header
{
	size_t lines;
	Modifier *modifiers;
}

struct List
{
	size_t num_elems;
	String *elems;
	size_t length;
	ListFlag flag;
}

/* intermediate struct for a PSYC packet */
struct Packet
{
	Header routing;	///< Routing header.
	Header entity;	///< Entity header.
	String method;
	String data;
	size_t routingLength;	///< Length of routing part.
	size_t contentLength;	///< Length of content part.
	size_t length;		///< Total length of packet.
	PacketFlag flag;
}

int psyc_version();

/** Check if a modifier needs length */
ModifierFlag checkModifierLength(Modifier *m);

/** Get the total length of a modifier. */
size_t getModifierLength(Modifier *m);

/** Create new modifier */
Modifier newModifier(char oper, String *name, String *value,
                                     ModifierFlag flag);

/** Create new modifier */
Modifier newModifier2(char oper,
                                      char *name, size_t namelen,
                                      char *value, size_t valuelen,
                                      ModifierFlag flag);

/** Check if a list needs length */
ListFlag checkListLength(List *list);

/** Get the total length of a list. */
ListFlag getListLength(List *list);

/** Check if a packet needs length */
PacketFlag checkPacketLength(Packet *p);

/** Calculate and set the length of packet parts and total packet length  */
size_t setPacketLength(Packet *p);

/** Create new list */
List newList(String *elems, size_t num_elems, ListFlag flag);

/** Create new packet */
Packet newPacket(Header *routing,
                                 Header *entity,
                                 String *method, String *data,
                                 PacketFlag flag);

/** Create new packet */
Packet newPacket2(Modifier *routing, size_t routinglen,
                                  Modifier *entity, size_t entitylen,
                                  char *method, size_t methodlen,
                                  char *data, size_t datalen,
                                  PacketFlag flag);

/// Routing vars in alphabetical order.
extern (C) String routingVars[];
extern (C) MatchVar varTypes[];

/**
 * Get the type of variable name.
 */
Bool isRoutingVar(char *name, size_t len);

/**
 * Get the type of variable name.
 */
Type getVarType(char *name, size_t len);

/**
 * Checks if long keyword string inherits from short keyword string.
 */
int inherits(char *sho, size_t slen,
                  char *lon, size_t llen);

/**
 * Checks if short keyword string matches long keyword string.
 */
int matches(char *sho, size_t slen,
                 char *lon, size_t llen);

/**
 * Callback for text() that produces a value for a match.
 *
 * The application looks up a match such as _fruit from [_fruit] and
 * if found writes its current value from its variable store into the
 * outgoing buffer.. "Apple" for example. The template returns the
 * number of bytes written. 0 is a legal return value. Should the
 * callback return -1, text leaves the original template text as is.
 */
alias extern (C) int function (char *match, size_t  mlen, char **buffer, size_t *blen) textCB;

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
 * See also http://about..eu/text
 */
int text(char *_template, size_t  tlen,
              char **buffer, size_t *blen,
              textCB lookupValue,
              char *braceOpen, char *braceClose);

