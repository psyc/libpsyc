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

#include <stdio.h>
#include <lib.h>

int main() {
    if (psyc_matches(PSYC_C2ARG("_failure_delivery"),
		     PSYC_C2ARG("_failure_unsuccessful_delivery_death")))
	return 1;
    if (psyc_matches(PSYC_C2ARG("_failure"),
		     PSYC_C2ARG("_failure_unsuccessful_delivery_death")))
	return 2;
    if (psyc_matches(PSYC_C2ARG("_unsuccessful"),
		     PSYC_C2ARG("_failure_unsuccessful_delivery_death")))
	return 3;
    unless (psyc_matches(PSYC_C2ARG("_fail"),
			 PSYC_C2ARG("_failure_unsuccessful_delivery_death")))
	return 4;
    unless (psyc_matches(PSYC_C2ARG("_truthahn"),
			 PSYC_C2ARG("_failure_unsuccessful_delivery_death")))
	return 5;

    puts("psyc_matches passed all tests.");

    unless (psyc_inherits(PSYC_C2ARG("_failure_delivery"),
			  PSYC_C2ARG("_failure_unsuccessful_delivery_death")))
	return 11;
    if (psyc_inherits(PSYC_C2ARG("_failure_unsuccessful"),
		      PSYC_C2ARG("_failure_unsuccessful_delivery_death")))
	return 12;
    unless (psyc_inherits(PSYC_C2ARG("_fail"),
			  PSYC_C2ARG("_failure_unsuccessful_delivery_death")))
	return 13;

    puts("psyc_inherits passed all tests.");

    return 0; // passed all tests
}
