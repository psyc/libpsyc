#include <stdio.h>
#include "../include/psyc/lib.h"

int main() {
	if (PSYC_matches("_failure_delivery", 0, "_failure_unsuccessful_delivery_death", 0)) return -1;
	if (PSYC_matches("_failure_trash", 8, "_failure_unsuccessful_delivery_death", 0)) return -2;
	if (PSYC_matches("_unsuccessful", 0, "_failure_unsuccessful_delivery_death", 0)) return -3;
	if (PSYC_matches("_fail", 0, "_failure_unsuccessful_delivery_death", 0)) return -4;
	unless (PSYC_matches("_truthahn", 0, "_failure_unsuccessful_delivery_death", 0)) return -5;

	return 0; // passed all tests
}
