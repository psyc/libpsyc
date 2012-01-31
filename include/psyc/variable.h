/**
 * @file psyc/variable.h
 */

#ifndef PSYC_VARIABLE_H
#define PSYC_VARIABLE_H

#include "packet.h"

/// Routing variables in alphabetical order.
extern const PsycDictInt psyc_rvars[];

// Variable types in alphabetical order.
extern const PsycDictInt psyc_var_types[];

/// Method names in alphabetical order.
extern const PsycDictInt psyc_methods[];

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
 * Look up routing variable.
 */
static inline PsycRoutingVar
psyc_var_routing (const char *name, size_t len)
{
    return (PsycRoutingVar) psyc_dict_lookup((PsycDict *)psyc_rvars,
				       psyc_rvars_num, name, len, PSYC_NO);
}

/**
 * Get the type of variable name.
 */
static inline PsycType
psyc_var_type (const char *name, size_t len)
{
    return (PsycType) psyc_dict_lookup((PsycDict *)psyc_var_types,
				       psyc_var_types_num, name, len, PSYC_YES);
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
