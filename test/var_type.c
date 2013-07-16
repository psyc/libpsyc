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
    for (i = 0; i < psyc_var_types_num; i++)
	if (psyc_var_type(PSYC_S2ARG(psyc_var_types[i].key))
	    != psyc_var_types[i].value)
	    return i + 1;

    unless (psyc_var_type(PSYC_C2ARG("_list"))) return 51;
    unless (psyc_var_type(PSYC_C2ARG("_list_foo"))) return 52;
    unless (psyc_var_type(PSYC_C2ARG("_color_red"))) return 53;

    if (psyc_var_type(PSYC_C2ARG("_last"))) return 104;
    if (psyc_var_type(PSYC_C2ARG("_lost_foo"))) return 105;
    if (psyc_var_type(PSYC_C2ARG("_colorful"))) return 106;
    if (psyc_var_type(PSYC_C2ARG("_foo"))) return 107;
    if (psyc_var_type(PSYC_C2ARG("bar"))) return 108;
    if (psyc_var_type(PSYC_C2ARG("______"))) return 109;
    if (psyc_var_type(PSYC_C2ARG("_"))) return 110;

    puts("psyc_var_type passed all tests.");
    return 0; // passed all tests
}
