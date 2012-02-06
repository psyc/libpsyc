/**
 * @file psyc/variable.h
 */

#ifndef PSYC_VARIABLE_H
#define PSYC_VARIABLE_H

#include "match.h"
#include "packet.h"

/// Routing variables in alphabetical order.
extern const PsycMapInt psyc_rvars[];

// Variable types in alphabetical order.
extern const PsycMapInt psyc_var_types[];

/// Method names in alphabetical order.
extern const PsycMapInt psyc_methods[];

extern const size_t psyc_rvars_num;
extern const size_t psyc_var_types_num;
extern const size_t psyc_methods_num;

typedef enum {
    PSYC_RVAR_UNKNOWN,

    PSYC_RVAR_AMOUNT_FRAGMENTS,
    PSYC_RVAR_CONTEXT,
    PSYC_RVAR_COUNTER,
    PSYC_RVAR_FRAGMENT,
    PSYC_RVAR_SOURCE,
    PSYC_RVAR_SOURCE_RELAY,
    PSYC_RVAR_TAG,
    PSYC_RVAR_TAG_RELAY,
    PSYC_RVAR_TARGET,
    PSYC_RVAR_TARGET_RELAY,

    PSYC_RVARS_NUM,
} PsycRoutingVar;

/**
 * Variable types.
 *
 * This enum lists PSYC variable types that
 * this library is capable of checking for
 * validity. Other variable types are treated
 * as opaque data.
 */
typedef enum {
    PSYC_TYPE_UNKNOWN,
    PSYC_TYPE_AMOUNT,
    PSYC_TYPE_COLOR,
    PSYC_TYPE_COUNTER,
    PSYC_TYPE_DATE,
    PSYC_TYPE_DEGREE,
    PSYC_TYPE_DICT,
    PSYC_TYPE_ENTITY,
    PSYC_TYPE_FLAG,
    PSYC_TYPE_LANGUAGE,
    PSYC_TYPE_LIST,
    PSYC_TYPE_NICK,
    PSYC_TYPE_PAGE,
    PSYC_TYPE_STRUCT,
    PSYC_TYPE_TIME,
    PSYC_TYPE_UNIFORM,
} PsycType;

/**
 * Look up routing variable.
 */
static inline PsycRoutingVar
psyc_var_routing (const char *name, size_t len)
{
    return (PsycRoutingVar)
	psyc_map_lookup((PsycMap*)psyc_rvars, psyc_rvars_num, name, len, PSYC_NO);
}

/**
 * Get the type of variable name.
 */
static inline PsycType
psyc_var_type (const char *name, size_t len)
{
    return (PsycType)
	psyc_map_lookup((PsycMap*)psyc_var_types, psyc_var_types_num,
			name, len, PSYC_YES);
}

/**
 * Is this a list variable name?
 */
static inline PsycBool
psyc_var_is_list (const char *name, size_t len)
{
    return len < 5 || memcmp(name, "_list", 5) != 0 || (len > 5 && name[5] != '_')
	? PSYC_FALSE : PSYC_TRUE;
}

PsycMethod
psyc_method (char *method, size_t methodlen, PsycMethod *family, unsigned int *flag);

#endif
