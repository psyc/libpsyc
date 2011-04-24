/* this is needed to compile the library, not to use it */

#ifndef PSYC_LIB_H
# define PSYC_LIB_H

#include "../psyc.h"
#include "./debug.h"
#include <sys/cdefs.h>

/* perlisms for readability */
#define unless(COND)    if (!(COND))
#define until(COND)     while (!(COND))

#if !defined(_GNU_SOURCE) && !defined(__FBSDID)
void * memmem(const void *l, size_t l_len, const void *s, size_t s_len);
#endif

#endif // PSYC_LIB_H
