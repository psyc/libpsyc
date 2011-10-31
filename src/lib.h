/* this is needed to compile the library, not to use it */

#ifndef PSYC_LIB_H
# define PSYC_LIB_H

#include <psyc.h>
#include <assert.h>
#include "debug.h"

/* perlisms for readability */
#define unless(COND)    if (!(COND))
#define until(COND)     while (!(COND))

#if !defined(__USE_GNU) && !(defined(__FBSDID) && defined(__BSD_VISIBLE))
void * memmem(const void *l, size_t l_len, const void *s, size_t s_len);
#endif

#if !defined(__USE_GNU) && !(defined(__FBSDID) && defined(__BSD_VISIBLE))
int itoa(int number, char* out, int base);
#endif

#endif // PSYC_LIB_H
