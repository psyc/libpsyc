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

#ifndef PSYC_MATCH_H
#define PSYC_MATCH_H

typedef struct {
    PsycString key;
    void *value;
} PsycMap;

typedef struct {
    PsycString key;
    intptr_t value;
} PsycMapInt;

/**
 * Checks if long keyword string inherits from short keyword string
 * according to the principles of PSYC keyword inheritance.
 * See about:inheritance.
 *
 * @param sho Shorter string
 * @param slen Shorter string length
 * @param lon Longer string
 * @param llen Longer string length
 */
int
psyc_inherits (char *sho, size_t slen, char *lon, size_t llen);

/**
 * Checks if short keyword string matches long keyword string
 * according to the principles of PSYC keyword matching.
 * See about:psycmatch.
 *
 * @param sho Shorter string
 * @param slen Shorter string length
 * @param lon Longer string
 * @param llen Longer string length
 */
int
psyc_matches (char *sho, size_t slen, char *lon, size_t llen);

/**
 * Look up value associated with a key in a map.
 *
 * @param map The dictionary to search, should be ordered alphabetically.
 * @param size Size of map.
 * @param key Key to look for.
 * @param keylen Length of key.
 * @param inherit If true, also look for anything inheriting from key,
 *                otherwise only exact matches are returned.
 *
 * @return The value of the entry if found, or NULL if not found.
 */
void *
psyc_map_lookup (const PsycMap *map, size_t size,
		 const char *key, size_t keylen, PsycBool inherit);

/**
 * Look up value associated with a key in a map with integer values.
 * @see psyc_map_lookup
 */
static inline intptr_t
psyc_map_lookup_int (const PsycMapInt *map, size_t size,
		     const char *key, size_t keylen, PsycBool inherit)
{
    return (intptr_t) psyc_map_lookup((PsycMap *) map, size, key, keylen, inherit);
}

#endif
