#include <psyc.h>
#include <stdio.h>
#include <lib.h>

int main()
{
    PsycMethod family = 0;
    unsigned int flag = 0;

    int i;
    for (i = 0; i < psyc_methods_num; i++)
	if (psyc_method(PSYC_S2ARG(psyc_methods[i].key), &family, &flag)
	    != psyc_methods[i].value)
	    return i + 1;

    if (psyc_method(PSYC_C2ARG("_echo_hallo"), &family, &flag) != PSYC_MC_UNKNOWN
	&& family != PSYC_MC_ECHO)
	return 101;

    if (psyc_method(PSYC_C2ARG("_foo_bar"), &family, &flag) != PSYC_MC_UNKNOWN)
	return 102;

    if (psyc_method(PSYC_C2ARG("_warning_test"), &family, &flag) != PSYC_MC_WARNING
	&& family != PSYC_MC_WARNING)
	return 103;

    printf("psyc_method passed all tests.\n");
    return 0;
}
