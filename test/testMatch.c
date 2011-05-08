#include <stdio.h>
#include <lib.h>

int main() {
	if (psyc_matches(PSYC_C2ARG("_failure_delivery"), PSYC_C2ARG("_failure_unsuccessful_delivery_death"))) return 1;
	if (psyc_matches(PSYC_C2ARG("_failure"), PSYC_C2ARG("_failure_unsuccessful_delivery_death"))) return 2;
	if (psyc_matches(PSYC_C2ARG("_unsuccessful"), PSYC_C2ARG("_failure_unsuccessful_delivery_death"))) return 3;
	unless (psyc_matches(PSYC_C2ARG("_fail"), PSYC_C2ARG("_failure_unsuccessful_delivery_death"))) return 4;
	unless (psyc_matches(PSYC_C2ARG("_truthahn"), PSYC_C2ARG("_failure_unsuccessful_delivery_death"))) return 5;

	puts("psyc_matches passed all tests.");

	unless (psyc_inherits(PSYC_C2ARG("_failure_delivery"), PSYC_C2ARG("_failure_unsuccessful_delivery_death"))) return 11;
	if (psyc_inherits(PSYC_C2ARG("_failure_unsuccessful"), PSYC_C2ARG("_failure_unsuccessful_delivery_death"))) return 12;
	unless (psyc_inherits(PSYC_C2ARG("_fail"), PSYC_C2ARG("_failure_unsuccessful_delivery_death"))) return 13;

	puts("psyc_inherits passed all tests.");

	return 0; // passed all tests
}
