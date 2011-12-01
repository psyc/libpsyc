#include <stdio.h>

#include <lib.h>
#include <psyc/render.h>
#include <psyc/syntax.h>

#define myUNI	"psyc://10.100.1000/~ludwig"

/* example renderer generating a presence packet */
int
testPresence (char *avail, int availlen,
	      char *desc, int desclen,
	      char *rendered, uint8_t verbose)
{
    PsycModifier routing[1];
    psyc_modifier_init(&routing[0], PSYC_OPERATOR_SET,
		       PSYC_C2ARG("_context"),
		       PSYC_C2ARG(myUNI), PSYC_MODIFIER_ROUTING);

    PsycModifier entity[2];
    // presence is to be assigned permanently in distributed state
    psyc_modifier_init(&entity[0], PSYC_OPERATOR_ASSIGN,
		       PSYC_C2ARG("_degree_availability"),
		       avail, availlen, PSYC_MODIFIER_CHECK_LENGTH);
    psyc_modifier_init(&entity[1], PSYC_OPERATOR_ASSIGN,
		       PSYC_C2ARG("_description_presence"),
		       desc, desclen, PSYC_MODIFIER_CHECK_LENGTH);

    PsycPacket packet;
    psyc_packet_init(&packet, routing, PSYC_NUM_ELEM(routing),
		     entity, PSYC_NUM_ELEM(entity),
		     PSYC_C2ARG("_notice_presence"),
		     NULL, 0,
		     PSYC_STATE_NOOP,
		     PSYC_PACKET_CHECK_LENGTH);

    char buffer[512];
    psyc_render(&packet, buffer, sizeof(buffer));
    if (verbose)
	printf("%.*s\n", (int)packet.length, buffer);
    return strncmp(rendered, buffer, packet.length);
}

int
testList (const char *rendered, uint8_t verbose)
{
    PsycModifier routing[2];
    psyc_modifier_init(&routing[0], PSYC_OPERATOR_SET,
		       PSYC_C2ARG("_source"),
		       PSYC_C2ARG(myUNI), PSYC_MODIFIER_ROUTING);
    psyc_modifier_init(&routing[1], PSYC_OPERATOR_SET,
		       PSYC_C2ARG("_context"),
		       PSYC_C2ARG(myUNI), PSYC_MODIFIER_ROUTING);

    PsycString elems_text[] = {
	PSYC_C2STR("foo"),
	PSYC_C2STR("bar"),
	PSYC_C2STR("baz"),
    };

    PsycString elems_bin[] = {
	PSYC_C2STR("foo"),
	PSYC_C2STR("b|r"),
	PSYC_C2STR("baz\nqux"),
    };

    PsycList list_text, list_bin;
    psyc_list_init(&list_text, elems_text,
		   PSYC_NUM_ELEM(elems_text), PSYC_LIST_CHECK_LENGTH);
    psyc_list_init(&list_bin, elems_bin,
		   PSYC_NUM_ELEM(elems_bin), PSYC_LIST_CHECK_LENGTH);

    char buf_text[32], buf_bin[32];
    psyc_render_list(&list_text, buf_text, sizeof(buf_text));
    psyc_render_list(&list_bin, buf_bin, sizeof(buf_bin));

    PsycModifier entity[2];
    psyc_modifier_init(&entity[0], PSYC_OPERATOR_SET,
		       PSYC_C2ARG("_list_text"),
		       buf_text, list_text.length, list_text.flag);
    psyc_modifier_init(&entity[1], PSYC_OPERATOR_SET,
		       PSYC_C2ARG("_list_binary"),
		       buf_bin, list_bin.length, list_bin.flag);

    PsycPacket packet;
    psyc_packet_init(&packet, routing, PSYC_NUM_ELEM(routing),
		     entity, PSYC_NUM_ELEM(entity),
		     PSYC_C2ARG("_test_list"),
		     PSYC_C2ARG("list test"),
		     PSYC_STATE_NOOP,
		     PSYC_PACKET_CHECK_LENGTH);

    char buffer[512];
    psyc_render(&packet, buffer, sizeof(buffer));
    if (verbose)
	printf("%.*s\n", (int)packet.length, buffer);
    return strncmp(rendered, buffer, packet.length);
}

int
main (int argc, char **argv)
{
    uint8_t verbose = argc > 1;

    if (testPresence(PSYC_C2ARG("_here"), PSYC_C2ARG("I'm omnipresent right now"), "\
:_context\t" myUNI "\n\
\n\
=_degree_availability\t_here\n\
=_description_presence\tI'm omnipresent right now\n\
_notice_presence\n\
|\n", verbose))
	return 1;

    if (testList("\
:_source	psyc://10.100.1000/~ludwig\n\
:_context	psyc://10.100.1000/~ludwig\n\
85\n\
:_list_text	|foo|bar|baz\n\
:_list_binary 21	3 foo|3 b|r|7 baz\n\
qux\n\
_test_list\n\
list test\n\
|\n", verbose))
	return 2;

    puts("psyc_render passed all tests.");

    return 0;
}
