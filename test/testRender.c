#include <stdlib.h>
#include "../include/psyc/lib.h"
#include "../include/psyc/render.h"

#define myUNI	"psyc://10.100.1000/~ludwig"

/* example renderer generating a presence packet */
int writePresence(const char *avail, int availlen, const char *desc, int desclen) {
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

	(void) PSYC_renderVar(pb, "_context", 0,
				 myUNI, sizeof(myUNI), PSYC_RENDER_ROUTING, 0);

	// the first call to PSYC_renderHeader() without PSYC_RENDER_ROUTING adds the
	// extra newline to the buffer. later vars with PSYC_RENDER_ROUTING cause an error.
	(void) PSYC_renderVar(pb, "_degree_availability", 0, avail, availlen, 0, C_GLYPH_OPERATOR_ASSIGN);
	(void) PSYC_renderVar(pb, "_description_presence", 0, desc, desclen, 0, C_GLYPH_OPERATOR_ASSIGN);
	// presence is to be assigned permanently in distributed state

	(void) PSYC_renderBody(pb, "_notice_presence", 0,
				   NULL, 0);  // no data in packet
//	(void) PSYC_doneRender(pb, &t, &l);
//	write(stdout, t, l);
	free(pb);
	return 0;
}


int main() {
	return writePresence("_here", 0, "I'm omnipresent right now", 0);
}
