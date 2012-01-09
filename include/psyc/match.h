typedef struct {
    PsycString key;
    void *value;
} PsycDict;

typedef struct {
    PsycString key;
    intptr_t value;
} PsycDictInt;

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
 * Look up value associated with a key in a dictionary.
 *
 * @param dict The dictionary to search, should be ordered alphabetically.
 * @param size Size of dict.
 * @param key Key to look for.
 * @param keylen Length of key.
 * @param inherit If true, also look for anything inheriting from key,
 *                otherwise only exact matches are returned.
 *
 * @return The value of the entry if found, or NULL if not found.
 */

void *
psyc_dict_lookup (const PsycDict *dict, size_t size,
		  const char *key, size_t keylen, PsycBool inherit);

/**
 * Look up value associated with a key in a dictionary of integers.
 * @see psyc_dict_lookup
 */
static inline intptr_t
psyc_dict_lookup_int (const PsycDictInt * dict, size_t size,
		      const char *key, size_t keylen, PsycBool inherit)
{
    return (intptr_t) psyc_dict_lookup((PsycDict *) dict, size, key, keylen, inherit);
}
