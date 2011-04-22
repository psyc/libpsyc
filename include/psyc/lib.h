/* this is needed to compile the library, not to use it */

#ifndef PSYC_LIB_H
# define PSYC_LIB_H

#include "../psyc.h"
#include "./debug.h"

/* perlisms for readability */
#define unless(COND)    if (!(COND))
#define until(COND)     while (!(COND))

#ifndef _GNU_SOURCE
void *memmem(const void *haystack, size_t haystacklen,
	     const void *needle,   size_t needlelen);
#endif

#endif // PSYC_LIB_H
