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
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <lib.h>

int main() {
    int i;

    for (i = 0; i < psyc_rvars_num; i++)
	if (psyc_var_routing(PSYC_S2ARG(psyc_rvars[i].key)) != psyc_rvars[i].value)
	    return i + 1;

    if (psyc_var_routing(PSYC_C2ARG("_source_foo"))) return 3;
    if (psyc_var_routing(PSYC_C2ARG("_sourcherry"))) return 4;
    if (psyc_var_routing(PSYC_C2ARG("_sour"))) return 5;
    if (psyc_var_routing(PSYC_C2ARG("_foo"))) return 6;
    if (psyc_var_routing(PSYC_C2ARG("bar"))) return 7;
    if (psyc_var_routing(PSYC_C2ARG("_"))) return 8;

    puts("psyc_var_routing passed all tests.");
    return 0;
}
