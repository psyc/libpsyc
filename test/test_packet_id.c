#include <stdlib.h>
#include <stdio.h>

#include <psyc.h>
#include <psyc/packet.h>
#include <psyc/render.h>

int
packet_id (char *context, size_t contextlen,
	   char *source, size_t sourcelen,
	   char *target, size_t targetlen,
	   char *counter, size_t counterlen,
	   char *fragment, size_t fragmentlen,
	   char *result, size_t resultlen)
{
    size_t idlen = psyc_packet_id_length(contextlen, sourcelen, targetlen,
					 counterlen, fragmentlen);
    char *id = malloc(idlen);
    psyc_render_packet_id(context, contextlen,
			  source, sourcelen,
			  target, targetlen,
			  counter, counterlen,
			  fragment, fragmentlen,
			  id, idlen);
    printf("%.*s\n", (int)idlen, id);
    int ret = idlen == resultlen && memcmp(result, id, idlen) == 0;
    free(id);
    return ret;
}

int
main (int argc, char **argv)
{
    if (!packet_id(PSYC_C2ARG(""),
		   PSYC_C2ARG("psyc://example.net/~alice"),
		   PSYC_C2ARG("psyc://example.net/~bob"),
		   PSYC_C2ARG("1337"),
		   PSYC_C2ARG("42"),
		   PSYC_C2ARG("||psyc://example.net/~alice|psyc://example.net/~bob"
			      "|1337|42")))
	return 1;

    if (!packet_id(PSYC_C2ARG("psyc://example.net/@bar"),
		   PSYC_C2ARG("psyc://example.net/~alice"),
		   PSYC_C2ARG(""),
		   PSYC_C2ARG("1337"),
		   PSYC_C2ARG("42"),
		   PSYC_C2ARG("|psyc://example.net/@bar|psyc://example.net/~alice|"
			      "|1337|42")))
	return 2;

    if (!packet_id(PSYC_C2ARG("psyc://example.net/@bar"),
		   PSYC_C2ARG(""),
		   PSYC_C2ARG("psyc://example.net/~alice"),
		   PSYC_C2ARG("1337"),
		   PSYC_C2ARG("42"),
		   PSYC_C2ARG("|psyc://example.net/@bar||psyc://example.net/~alice"
			      "|1337|42")))
	return 3;

    if (!packet_id(PSYC_C2ARG("psyc://example.net/@bar"),
		   PSYC_C2ARG(""),
		   PSYC_C2ARG(""),
		   PSYC_C2ARG(""),
		   PSYC_C2ARG(""),
		   PSYC_C2ARG("|psyc://example.net/@bar||||")))
	return 4;

    return 0;
}
