/** @file d/psyc/common.d
 *
 * @brief Main PSYC interface providing crucial functionality.
*/



module psyc.common;

const EPOCH = 1440444041;      // 2015-08-24 21:20:41 CET (Monday)

extern (C):

/+enum Bool
{
	FALSE = 0,
	TRUE = 1,
}+/

alias bool Bool;
static const FALSE = false;
static const TRUE  = true;

/**
 * PSYC packet parts.
 */
enum Part
{
	RESET = -1,
	ROUTING = 0,
	LENGTH = 1,
	CONTENT = 2,
	METHOD = 3,
	DATA = 4,
	END = 5,
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
	UNKNOWN,
	AMOUNT,
	COLOR,
	DATE,
	DEGREE,
	ENTITY,
	FLAG,
	LANGUAGE,
	LIST,
	NICK,
	PAGE,
	UNIFORM,
	TIME,
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

/+
struct String
{
	size_t length;
	ubyte *ptr;
}+/

alias ubyte[] String;

struct MatchVar
{
	String name;
	int value;
}

int psyc_version()
{
	return 1;
}

/// Routing vars in alphabetical order.
extern (C) String routingVars[];
extern (C) MatchVar varTypes[];

/**
 * Get the type of variable name.
 */
Bool psyc_isRoutingVar (char[]* name);

bool isRoutingVar (char[] name)
{
	return psyc_isRoutingVar(&name);
}

/**
 * Get the type of variable name.
 */
Type psyc_getVarType(char *name, size_t len);

alias psyc_getVarType getVarType;

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


