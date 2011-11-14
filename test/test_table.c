#include <stdio.h>
#include <stdlib.h>

#include <psyc/parse.h>

int
parse_table (char *buf, size_t buflen)
{
    printf(">> %.*s\n", (int)buflen, buf);

    int ret;
    PsycString elem;
    PsycParseTableState state;
    psyc_parse_table_state_init(&state);
    psyc_parse_table_buffer_set(&state, buf, buflen);

    do {
	ret = psyc_parse_table(&state, &elem);
	switch (ret) {
	case PSYC_PARSE_TABLE_WIDTH:
	    printf("width: %ld\n", elem.length);
	    break;
#ifdef PSYC_PARSE_TABLE_HEAD
	case PSYC_PARSE_TABLE_NAME_KEY:
	    printf("name key: %.*s\n", (int)PSYC_S2ARG2(elem));
	    break;
	case PSYC_PARSE_TABLE_NAME_VALUE:
	    printf("name val: %.*s\n", (int)PSYC_S2ARG2(elem));
	    break;
#endif
	case PSYC_PARSE_TABLE_KEY_END:
	    ret = 0;
	case PSYC_PARSE_TABLE_KEY:
	    printf("key: %.*s\n", (int)PSYC_S2ARG2(elem));
	    break;
	case PSYC_PARSE_TABLE_VALUE_END:
	    ret = 0;
	case PSYC_PARSE_TABLE_VALUE:
	    printf("val: %.*s\n", (int)PSYC_S2ARG2(elem));
	    break;
	default:
	    printf("err: %d\n", ret);
	}
    } while (ret > 0);
    return ret == 0;
}

int
main (int argc, char **argv)
{

#ifdef PSYC_PARSE_TABLE_HEAD
    if (!parse_table(PSYC_C2ARG("*2|_key|_val1|_val2 |_foo|bar|baz|_aaa|bbb|ccc")))
	return 1;
#endif

    if (!parse_table(PSYC_C2ARG("*2 |_foo|bar|baz|_aaa|bbb|ccc")))
	return 2;

#ifdef PSYC_PARSE_TABLE_HEAD
    if (!parse_table(PSYC_C2ARG("*1|_key|_val1 |_foo|bar|_baz|aaa|_bbb|ccc")))
	return 1;
#endif

    if (!parse_table(PSYC_C2ARG("*1 |_foo|bar|_baz|aaa|_bbb|ccc")))
	return 3;

#ifdef PSYC_PARSE_TABLE_HEAD
    if (!parse_table(PSYC_C2ARG("*0|_key |foo|bar|baz|aaa|bbb|ccc")))
	return 4;
#endif

    if (!parse_table(PSYC_C2ARG("*0 |foo|bar|baz|aaa|bbb|ccc")))
	return 4;

    if (!parse_table(PSYC_C2ARG("|foo|bar|baz|aaa|bbb|ccc")))
	return 5;

    return 0;
}
