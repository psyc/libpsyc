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
 * Checks if long keyword string inherits from short keyword string.
 */
int
psyc_inherits (char *sho, size_t slen, char *lon, size_t llen);

/**
 * Checks if short keyword string matches long keyword string.
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
psyc_map_lookup_int (const PsycMapInt * map, size_t size,
		      const char *key, size_t keylen, PsycBool inherit)
{
    return (intptr_t) psyc_map_lookup((PsycMap *) map, size, key, keylen, inherit);
}

#endif
