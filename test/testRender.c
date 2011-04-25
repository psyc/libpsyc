#include <stdlib.h>
#include <unistd.h>
#include "../include/psyc/lib.h"
#include "../include/psyc/render.h"

#define myUNI	"psyc://10.100.1000/~ludwig"

/* example renderer generating a presence packet */
int writePresence(const char *avail, int availlen, const char *desc, int desclen)
{
	PSYC_Modifier routing[] = {
		PSYC_newModifier2(C_GLYPH_OPERATOR_SET, PSYC_C2ARG("_source"), PSYC_C2ARG(myUNI),
		                  PSYC_MODIFIER_ROUTING),
		PSYC_newModifier2(C_GLYPH_OPERATOR_SET, PSYC_C2ARG("_context"), PSYC_C2ARG(myUNI),
		                  PSYC_MODIFIER_CHECK_LENGTH),
	};

	PSYC_Modifier entity[] = {
		// presence is to be assigned permanently in distributed state
		PSYC_newModifier2(C_GLYPH_OPERATOR_ASSIGN, PSYC_C2ARG("_degree_availability"),
		                  avail, availlen, PSYC_PACKET_CHECK_LENGTH),
		PSYC_newModifier2(C_GLYPH_OPERATOR_ASSIGN, PSYC_C2ARG("_description_presence"),
		                  desc, desclen, PSYC_PACKET_CHECK_LENGTH),
	};

	PSYC_Packet packet = PSYC_newPacket2(routing, PSYC_NUM_ELEM(routing),
	                                     entity, PSYC_NUM_ELEM(entity),
	                                     PSYC_C2ARG("_notice_presence"),
	                                     NULL, 0,
	                                     //PSYC_C2ARG("foobar"),
	                                     PSYC_PACKET_CHECK_LENGTH);

	char buffer[512];
	PSYC_render(&packet, buffer, sizeof(buffer));
	write(0, buffer, packet.length);

/*
	PSYC_RenderState *pb; // a chunk of mem will host both struct and buf1
//	char *t;
//	unsigned int l;

	unless (availlen) availlen = strlen(avail);
	unless (desclen) desclen = strlen(desc);

	if (!(pb = malloc(512))) {
		P0(("Out of memory\n"))
		return -1;
	}
//	if (PSYC_initBuffer(pb, WHATEVER)) die("PSYC_initBuffer hates me");

	(void) PSYC_renderModifier(pb, "_context", 0,
				 myUNI, sizeof(myUNI), PSYC_RENDER_ROUTING, 0);

	// the first call to PSYC_renderHeader() without PSYC_RENDER_ROUTING adds the
	// extra newline to the buffer. later vars with PSYC_RENDER_ROUTING cause an error.
	(void) PSYC_renderModifier(pb, "_degree_availability", 0, avail, availlen, 0, C_GLYPH_OPERATOR_ASSIGN);
	(void) PSYC_renderModifier(pb, "_description_presence", 0, desc, desclen, 0, C_GLYPH_OPERATOR_ASSIGN);
	// presence is to be assigned permanently in distributed state

	(void) PSYC_renderBody(pb, "_notice_presence", 0,
				   NULL, 0);  // no data in packet
//	(void) PSYC_doneRender(pb, &t, &l);
//	write(stdout, t, l);
	free(pb);
*/
	return 0;
}


int main() {
	return writePresence(PSYC_C2ARG("_here"), PSYC_C2ARG("I'm omnipresent right now"));
}
