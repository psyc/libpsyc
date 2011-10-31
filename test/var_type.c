#include <psyc.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <lib.h>

int main() {
	unless (psyc_var_type(PSYC_C2ARG("_list"))) return 1;
	unless (psyc_var_type(PSYC_C2ARG("_list_foo"))) return 2;
	unless (psyc_var_type(PSYC_C2ARG("_color_red"))) return 3;
	if (psyc_var_type(PSYC_C2ARG("_last"))) return 4;
	if (psyc_var_type(PSYC_C2ARG("_lost_foo"))) return 5;
	if (psyc_var_type(PSYC_C2ARG("_colorful"))) return 6;
	if (psyc_var_type(PSYC_C2ARG("_foo"))) return 7;
	if (psyc_var_type(PSYC_C2ARG("bar"))) return 8;
	if (psyc_var_type(PSYC_C2ARG("______"))) return 9;
	if (psyc_var_type(PSYC_C2ARG("_"))) return 10;

	puts("psyc_var_type passed all tests.");
	return 0; // passed all tests
}
