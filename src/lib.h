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
