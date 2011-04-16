#include <string.h>
#include <stdint.h>

#ifdef TEST
# include <stdio.h>
# define PT(args) printf args;
#else
# define PT(args)
#endif

int PSYC_matches(uint8_t* sho, unsigned int slen,
	      uint8_t* lon, unsigned int llen) {
	uint8_t *s, *l, *se, *le;

	if (!slen) slen = strlen(sho);
	if (!llen) llen = strlen(lon);

	if (slen == 0 || *sho != '_' ||
	    llen == 0 || *lon != '_') {
		PT(("Please use long format keywords (compact ones would be faster, I know..)\n"))
		return -2;
	}

	if (slen > llen) {
		PT(("The long string is shorter than the short one.\n"))
		return -3;
	}

	if (slen == llen) {
		if (!strcmp(sho, lon)) {
			PT(("Identical arguments.\n"))
			return 0;
		}
		PT(("Same length but different.\nNo match, but they could be related or have a common type.\n"))
		return -4;
	}
	PT(("*start short '%s' long '%s'\n", sho, lon))

	se = sho+slen;
	le = lon+llen;
	*se = *le = '_';
	sho++; lon++;
	while(s = strchr(sho, '_')) {
		*s = 0;
		PT(("sho goes '%c' and lon goes '%c'\n", *sho, *lon))
		while(l = strchr(lon, '_')) {
			*l = 0;
			PT(("strcmp short '%s' long '%s'\n", sho, lon))
			if (!strcmp(sho, lon)) goto foundone;
			if (l == le) goto failed;
			*l = '_';
			lon = ++l;
		}
		goto failed;
foundone:
		PT(("found: short '%s' long '%s'\n", sho, lon))
		if (s == se) goto success;
		*l = *s = '_';
		sho = ++s;
		lon = ++l;
	}
success:
	return 0;
failed:
	PT(("No, they don't match.\n"))
	return 1;
}

#ifdef TEST
int main(int argc, char **argv) {
	if (argc != 3) {
		printf("Usage: %s <short> <long>\n\nExample: %s _failure_delivery _failure_unsuccessful_delivery_death\n", argv[0], argv[0]);
		return -1;
	}
	if (PSYC_matches((uint8_t*) argv[1], 0, (uint8_t*) argv[2], 0) == 0)
	    printf("Yes, they match!\n");
}
#endif
