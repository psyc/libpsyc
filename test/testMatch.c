#include <stdio.h>
#include "../include/psyc/lib.h"

int main() {
	if (psyc_matches("_failure_delivery", 0, "_failure_unsuccessful_delivery_death", 0)) return 1;
	if (psyc_matches("_failure_trash", 8, "_failure_unsuccessful_delivery_death", 0)) return 2;
	if (psyc_matches("_unsuccessful", 0, "_failure_unsuccessful_delivery_death", 0)) return 3;
	unless (psyc_matches("_fail", 0, "_failure_unsuccessful_delivery_death", 0)) return 4;
	unless (psyc_matches("_truthahn", 0, "_failure_unsuccessful_delivery_death", 0)) return 5;

	puts("psyc_matches passed all tests.");

	unless (psyc_inherits("_failure_delivery", 0, "_failure_unsuccessful_delivery_death", 0)) return 11;
	if (psyc_inherits("_failure_unsuccessful", 0, "_failure_unsuccessful_delivery_death", 0)) return 12;
	unless (psyc_inherits("_fail", 0, "_failure_unsuccessful_delivery_death", 0)) return 13;

	puts("psyc_inherits passed all tests.");

	return 0; // passed all tests
}
