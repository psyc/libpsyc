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
