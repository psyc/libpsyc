#include <stdlib.h>
#include <unistd.h>
#include "../include/psyc/lib.h"
#include "../include/psyc/render.h"
#include "../include/psyc/syntax.h"

#define myUNI	"psyc://10.100.1000/~ludwig"

/* example renderer generating a presence packet */
int testPresence(const char *avail, int availlen, const char *desc, int desclen, const char *rendered)
{
	psycModifier routing[] = {
//		psyc_newModifier2(C_GLYPH_OPERATOR_SET, PSYC_C2ARG("_source"), PSYC_C2ARG(myUNI),
//		                  PSYC_MODIFIER_ROUTING),
		psyc_newModifier2(C_GLYPH_OPERATOR_SET, PSYC_C2ARG("_context"), PSYC_C2ARG(myUNI),
		                  PSYC_MODIFIER_ROUTING),
	};

	psycModifier entity[] = {
		// presence is to be assigned permanently in distributed state
		psyc_newModifier2(C_GLYPH_OPERATOR_ASSIGN, PSYC_C2ARG("_degree_availability"),
		                  avail, availlen, PSYC_PACKET_CHECK_LENGTH),
		psyc_newModifier2(C_GLYPH_OPERATOR_ASSIGN, PSYC_C2ARG("_description_presence"),
		                  desc, desclen, PSYC_PACKET_CHECK_LENGTH),
	};

	psycPacket packet = psyc_newPacket2(routing, PSYC_NUM_ELEM(routing),
	                                     entity, PSYC_NUM_ELEM(entity),
	                                     PSYC_C2ARG("_notice_presence"),
	                                     NULL, 0,
	                                     //PSYC_C2ARG("foobar"),
	                                     PSYC_PACKET_CHECK_LENGTH);

	char buffer[512];
	psyc_render(&packet, buffer, sizeof(buffer));
	// write(0, buffer, packet.length);
	return strncmp(rendered, buffer, packet.length);
}

int main() {
	int rc = testPresence(PSYC_C2ARG("_here"), PSYC_C2ARG("I'm omnipresent right now"), "\
:_context\t" myUNI "\n\
\n\
=_degree_availability\t_here\n\
=_description_presence\tI'm omnipresent right now\n\
_notice_presence\n\
|\n");
	unless (rc) puts("psyc_render passed the test.");
	return rc;
}
