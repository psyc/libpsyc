#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
	char *sho, *lon, *s, *l, *se, *le;
	int slen, llen;

	if (argc != 3) {
		printf("Usage: %s <short> <long>\n\nExample: %s _failure_delivery _failure_unsuccessful_delivery_death\n", argv[0], argv[0]);
		return -1;
	}
	sho = argv[1];
	lon = argv[2];

	if (!(slen = strlen(sho)) || *sho != '_' ||
	    !(llen = strlen(lon)) || *lon != '_') {
		printf("Please use long format keywords (compact ones would be faster, I know..)\n");
		return -2;
	}

	if (slen > llen) {
		printf("The long string is shorter than the short one.\n");
		return -3;
	}

	if (slen == llen) {
		if (!strcmp(sho, lon)) {
			printf("Identical arguments.\n");
			return 0;
		}
		printf("Same length but different.\nNo match, but they could be related or have a common type.\n");
		return -4;
	}
	printf("*start short '%s' long '%s'\n", sho, lon);

	se = sho+slen;
	le = lon+llen;
	*se = *le = '_';
	sho++; lon++;
	while(s = strchr(sho, '_')) {
		*s = 0;
		printf("sho goes '%c' and lon goes '%c'\n", *sho, *lon);
		while(l = strchr(lon, '_')) {
			*l = 0;
			printf("strcmp short '%s' long '%s'\n", sho, lon);
			if (!strcmp(sho, lon)) goto foundone;
			if (l == le) goto failed;
			*l = '_';
			lon = ++l;
		}
		goto failed;
foundone:
		printf("found: short '%s' long '%s'\n", sho, lon);
		if (s == se) goto success;
		*l = *s = '_';
		sho = ++s;
		lon = ++l;
	}
success:
	printf("Yes, they match!\n");
	return 0;
failed:
	printf("No, they don't!\n");
	return 1;
}

