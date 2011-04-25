#define ALPHANUMS "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"

/** converts an integer to a string, using a base of 10 by default.
 *
 * if you NULL out the output buffer it will return the expected
 * output string length anyway.
 */
int itoa(int number, char* out, int base) {
	int t, count;
	char *p, *q;
	char c;

	p = q = out;
	if (base < 2 || base > 36) base = 10;

	do {
		t = number;
		number /= base;
		if (out) *p = ALPHANUMS[t+35 - number*base];
		p++;
	} while (number);

	if (t < 0) {
	     if (out) *p = '-';
	     p++;
	}
	count = p-out;
	if (out) {
		*p-- = '\0';
		while(q < p) {
			c = *p;
			*p-- = *q;
			*q++ = c;
		}
	}
	return count;
}

/* This little test program shows that itoa() is roughly 3 times faster
 * than sprintf  --lynX
 */
#ifdef CMDTOOL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
	char out[4404];
	int in[44];
	int c, i, j;

        if (argc < 3 || argc > sizeof(in)) {
                printf("Usage: %s <times> <numbers>+\n\nExample: %s 999999 123 234 345 -49 -21892\n", argv[0], argv[0]);
                return -1;
        }
	for (j=argc-1; j; j--) {
//		printf("Looking at arg #%d: %s\n", j, argv[j]);
		in[j] = atoi(argv[j]);
//		printf("Got %d: %d\n", j, in[j]);
	}
	for (i=in[1]; i; i--) {
		c = 0;
		for (j=argc-1; j>1; j--) {
# if 0
			// use good old sprintf
			c += sprintf(&out[c], " %d", in[j]);
# else
#  if 1
			// use the itoa implementation
			out[c++] = ' ';
			c += itoa(in[j], &out[c], 10);
#  else
			// just count the needed space
			c += itoa(in[j], NULL, 10) + 1;
#  endif
# endif
		}
	}
        printf("%d times, %d count, buffer len: %d, buffer: %s\n",
	    in[1], c, strlen(out), "<omitted>");	// out
	return 0;
}

#endif
