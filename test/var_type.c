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
