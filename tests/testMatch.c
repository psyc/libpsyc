#include <stdio.h>
#include "../include/psyc/lib.h"

int main() {
	if (PSYC_matches("_failure_delivery", 0, "_failure_unsuccessful_delivery_death", 0)) return -1;
	if (PSYC_matches("_failure_trash", 8, "_failure_unsuccessful_delivery_death", 0)) return -2;
	if (PSYC_matches("_unsuccessful", 0, "_failure_unsuccessful_delivery_death", 0)) return -3;
	unless (PSYC_matches("_fail", 0, "_failure_unsuccessful_delivery_death", 0)) return -4;
	unless (PSYC_matches("_truthahn", 0, "_failure_unsuccessful_delivery_death", 0)) return -5;

	puts("PSYC_matches passed all tests.");

	unless (PSYC_inherits("_failure_delivery", 0, "_failure_unsuccessful_delivery_death", 0)) return -11;
	if (PSYC_inherits("_failure_unsuccessful", 0, "_failure_unsuccessful_delivery_death", 0)) return -12;
	unless (PSYC_inherits("_fail", 0, "_failure_unsuccessful_delivery_death", 0)) return -13;

	puts("PSYC_inherits passed all tests.");

	return 0; // passed all tests
}
