#include "lib.h"

int psyc_inherits (char* sho, size_t slen,
                   char* lon, size_t llen) {

	// this allows to pass zero-terminated strings instead of providing
	// the length.. but we would be faster here if we expected the callee
	// to always use the PSYC_C2ARG() macro instead. additionally, the
	// empty string would then be fully supported (in case you want that)
	// Disabled this, let's use that macro rather.
	//if (!slen) slen = strlen(sho);
	//if (!llen) llen = strlen(lon);

	if (slen == 0 || *sho != '_' ||
	    llen == 0 || *lon != '_') {
		P1(("Please use long format keywords (compact ones would be faster, I know..)\n"))
		return -2;
	}

	if (slen > llen) {
		P1(("The long string is shorter than the short one.\n"))
		return -3;
	}

	if (!strncmp(sho, lon, slen)) {
		/* according to PSYC spec we have hereby already proved
		 * inheritance. the following check is optional!
		 */
		if (llen > slen && lon[slen] != '_') {
			/* It is illegal to introduce a keyword family
			 * that starts just like an existing one. Since
			 * _failure exists, you can't use _fail. But
			 * implementations are not required to recognize
			 * that.
			 */
			P1(("Illegal choice of keyword names!\n"))
			return -4;
		}
		return 0;
	}
	P4(("%.*s does not inherit from %.*s.\n", (int)llen, lon, (int)slen, sho))
	return 1;
}

int psyc_matches (char* sho, size_t slen,
                  char* lon, size_t llen) {
	char *s, *l, *se, *le;

	//if (!slen) slen = strlen(sho);
	//if (!llen) llen = strlen(lon);

	if (slen == 0 || *sho != '_' ||
	    llen == 0 || *lon != '_') {
		P1(("Please use long format keywords (compact ones would be faster, I know..)\n"))
		return -2;
	}

	if (slen > llen) {
		P1(("The long string is shorter than the short one.\n"))
		return -3;
	}

	if (slen == llen) {
		if (!strncmp(sho, lon, slen)) {
			P1(("Identical arguments.\n"))
			return 0;
		}
		P1(("Same length but different.\nNo match, but they could be related or have a common type.\n"))
		return -4;
	}
	P3(("# psyc_matches short '%.*s' in long '%.*s' ?\n", (int)slen, sho, (int)llen, lon))

	se = sho+slen;
	le = lon+llen;
	sho++; lon++; slen--; llen--;
	while(*sho && sho < se) {
		P3(("# comparing short '%.*s' (%d)\n", (int)slen, sho, (int)slen))
		unless (s = memchr(sho, '_', slen)) s = se;
		P4(("# sho goes '%c' and lon goes '%c'\n", *sho, (int)*lon))
		while(*lon && lon < le) {
			P3(("# against long '%.*s' (%d)\n", (int)llen, lon, (int)llen))
			unless (l = memchr(lon, '_', llen)) l = le;
			P3(("# %ld == %ld && !strncmp '%.*s', '%.*s'\n", s-sho, l-lon, (int)(s-sho), sho, (int)(s-sho), lon))
			if (l-lon == s-sho && !strncmp(sho, lon, s-sho)) goto foundone;
			P4(("# failed\n"))
			llen -= l-lon + 1;
			lon = ++l;
		}
		goto failed;
foundone:
		P3(("# found %ld of short '%.*s' and long '%.*s'\n", s-sho, (int)(s-sho), sho, (int)(s-sho), lon))
		llen -= l-lon;
		slen -= s-sho;
		sho = ++s;
		lon = ++l;
	}
	return 0;
failed:
	P4(("No, they don't match.\n"))
	return 1;
}

/**
 * Look up value associated with a key in a dictionary.
 */
void * psyc_dict_lookup (const PsycDict *dict, size_t size,
                         const char *key, size_t keylen,
                         PsycBool inherit, int8_t *matching)
{
	size_t cursor = 1;
	uint8_t i, m = 0;
	//memset(&matching, -1, sizeof matching);

	if (keylen < 2 || key[0] != '_')
		return 0;

	// first find the keywords with matching length
	for (i=0; i<size; i++)
		if (keylen == dict[i].key.length ||
		    (inherit && keylen > dict[i].key.length && key[dict[i].key.length] == '_'))
			matching[m++] = i;

	matching[m] = -1; // mark the end of matching indexes

	while (cursor < keylen && matching[0] >= 0) {
		for (i = m = 0; i < size; i++) {
			if (matching[i] < 0)
				break; // reached the end of possible matches
			if (cursor < dict[matching[i]].key.length &&
			    dict[matching[i]].key.data[cursor] == key[cursor])
				matching[m++] = matching[i]; // found a match, update matching indexes
			else if (cursor == dict[matching[i]].key.length && key[cursor] == '_')
				return dict[matching[0]].value; // _ after the end of a matching prefix
			else if (dict[matching[i]].key.data[cursor] > key[cursor])
				break; // passed the possible matches in alphabetical order in the dict
		}

		if (m < size)
			matching[m] = -1; // mark the end of matching indexes

		cursor++;
	}

	// return first match if found
	return matching[0] >= 0 ? dict[matching[0]].value : 0;
}

#ifdef CMDTOOL
int main(int argc, char **argv) {
	if (argc != 3) {
		printf("Usage: %s <short> <long>\n\nExample: %s _failure_delivery _failure_unsuccessful_delivery_death\n", argv[0], argv[0]);
		return -1;
	}
	if (psyc_matches(argv[1], 0, argv[2], 0) == 0)
	    printf("Yes, %s matches %s!\n", argv[1], argv[2]);
	if (psyc_inherits(argv[1], 0, argv[2], 0) == 0)
	    printf("Yes, %s inherits from %s!\n", argv[2], argv[1]);
}
#endif
