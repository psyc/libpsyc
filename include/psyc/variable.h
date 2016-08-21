/*
  This file is part of libpsyc.
  Copyright (C) 2011,2012 Carlo v. Loesch, Gabor X Toth, Mathias L. Baumann,
  and other contributing authors.

  libpsyc is free software: you can redistribute it and/or modify it under the
  terms of the GNU Affero General Public License as published by the Free
  Software Foundation, either version 3 of the License, or (at your option) any
  later version. As a special exception, libpsyc is distributed with additional
  permissions to link libpsyc libraries with non-AGPL works.

  libpsyc is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
  details.

  You should have received a copy of the GNU Affero General Public License and
  the linking exception along with libpsyc in a COPYING file.
*/

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
inline PsycRoutingVar
psyc_var_routing (const char *name, size_t len)
{
    return (PsycRoutingVar)
	psyc_map_lookup_int((PsycMapInt*)psyc_rvars, psyc_rvars_num, name, len,
			    PSYC_NO);
}

/**
 * Get the type of variable name.
 */
inline PsycType
psyc_var_type (const char *name, size_t len)
{
    return (PsycType)
	psyc_map_lookup_int((PsycMapInt*)psyc_var_types, psyc_var_types_num,
			    name, len, PSYC_YES);
}

/**
 * Is this a list variable name?
 */
inline PsycBool
psyc_var_is_list (const char *name, size_t len)
{
    return len < 5 || memcmp(name, "_list", 5) != 0 || (len > 5 && name[5] != '_')
	? PSYC_FALSE : PSYC_TRUE;
}

PsycMethod
psyc_method (char *method, size_t methodlen, PsycMethod *family, unsigned int *flag);

#endif
