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

#include <psyc.h>
#include <stdio.h>
#include <lib.h>

int main()
{
    PsycMethod family = 0;
    unsigned int flag = 0;

    int i;
    for (i = 0; i < psyc_methods_num; i++)
	if (psyc_method(PSYC_S2ARG(psyc_methods[i].key), &family, &flag)
	    != psyc_methods[i].value)
	    return i + 1;

    if (psyc_method(PSYC_C2ARG("_echo_hallo"), &family, &flag) != PSYC_MC_UNKNOWN
	&& family != PSYC_MC_ECHO)
	return 101;

    if (psyc_method(PSYC_C2ARG("_foo_bar"), &family, &flag) != PSYC_MC_UNKNOWN)
	return 102;

    if (psyc_method(PSYC_C2ARG("_warning_test"), &family, &flag) != PSYC_MC_WARNING
	&& family != PSYC_MC_WARNING)
	return 103;

    printf("psyc_method passed all tests.\n");
    return 0;
}
