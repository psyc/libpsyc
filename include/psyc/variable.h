#ifndef PSYC_VARIABLE_H

/**
 * @file psyc/variable.h
 */

/// Routing variables in alphabetical order.
extern const psycString psyc_routing_vars[];

// Variable types in alphabetical order.
extern const psycMatchVar psyc_var_types[];

extern const size_t psyc_routing_vars_num;
extern const size_t psyc_var_types_num;

/**
 * Is this a routing variable name?
 */
psycBool psyc_var_is_routing (const char *name, size_t len);

/**
 * Get the type of variable name.
 */
psycType psyc_var_type (const char *name, size_t len);

/**
 * Is this a list variable name?
 */
static inline
psycBool psyc_var_is_list (const char *name, size_t len)
{
	return len < 5 || memcmp(name, "_list", 5) != 0 ||
		(len > 5 && name[5] != '_') ? PSYC_FALSE : PSYC_TRUE;
}

#define PSYC_VARIABLE_H
#endif
