#include <stdlib.h>
#include <unistd.h>
#include "../include/psyc/lib.h"
#include "../include/psyc/render.h"
#include "../include/psyc/syntax.h"

#define myUNI	"psyc://10.100.1000/~ludwig"

/* example renderer generating a presence packet */
int writePresence(const char *avail, int availlen, const char *desc, int desclen)
{
	psycModifier routing[] = {
		psyc_newModifier2(C_GLYPH_OPERATOR_SET, PSYC_C2ARG("_source"), PSYC_C2ARG(myUNI),
		                  PSYC_MODIFIER_ROUTING),
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
	write(0, buffer, packet.length);

/*
	psycRenderState *pb; // a chunk of mem will host both struct and buf1
//	char *t;
//	unsigned int l;

	unless (availlen) availlen = strlen(avail);
	unless (desclen) desclen = strlen(desc);

	if (!(pb = malloc(512))) {
		P0(("Out of memory\n"))
		return -1;
	}
//	if (psyc_initBuffer(pb, WHATEVER)) die("psyc_initBuffer hates me");

	(void) psyc_renderModifier(pb, "_context", 0,
				 myUNI, sizeof(myUNI), PSYC_RENDER_ROUTING, 0);

	// the first call to psyc_renderHeader() without PSYC_RENDER_ROUTING adds the
	// extra newline to the buffer. later vars with PSYC_RENDER_ROUTING cause an error.
	(void) psyc_renderModifier(pb, "_degree_availability", 0, avail, availlen, 0, C_GLYPH_OPERATOR_ASSIGN);
	(void) psyc_renderModifier(pb, "_description_presence", 0, desc, desclen, 0, C_GLYPH_OPERATOR_ASSIGN);
	// presence is to be assigned permanently in distributed state

	(void) psyc_renderBody(pb, "_notice_presence", 0,
				   NULL, 0);  // no data in packet
//	(void) psyc_doneRender(pb, &t, &l);
//	write(stdout, t, l);
	free(pb);
*/
	return 0;
}


int main() {
	return writePresence(PSYC_C2ARG("_here"), PSYC_C2ARG("I'm omnipresent right now"));
}
