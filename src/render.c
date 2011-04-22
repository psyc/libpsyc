#include "psyc/lib.h"
#include "psyc/render.h"

int PSYC_renderHeader(PSYC_RenderState* r,
                      const char* name, size_t nlength,
                      const char* value, size_t vlength,
                      const PSYC_RenderHeaderFlag flags,
		      char modifier) {
	unless (nlength) nlength = strlen(name);
	// vlength 0 means an empty variable.. no cheating there
	unless (modifier) modifier = C_GLYPH_MODIFIER_SET;

	r->buffer[r->cursor++] = modifier;
	strncpy(&r->buffer[r->cursor], name, nlength);
	r->cursor += nlength;
	if (vlength) {
		r->buffer[r->cursor++] = '\t';
		strncpy(&r->buffer[r->cursor], value, vlength);
		r->cursor += vlength;
	}

	if (flags == PSYC_FLAG_ROUTING) {
		if (r->spot != 0) {
			P1(("Too late to add a routing variable!\n"))
			return -1;
		}
	} else if (r->spot == 0) {
		// add "\n000000000" to buffer
		// and make spot point to the first 0
	}
	return 0;
}

int PSYC_renderBody(PSYC_RenderState* render,
                    const char* method, size_t mlength,
                    const char* data, size_t dlength) {

	// find out if this packet needs a prepended length
	if (dlength == 1 && data[0] == C_GLYPH_PACKET_DELIMITER)
	    render->flag = PSYC_NEED_LENGTH;
	else if (dlength > 404)
	    render->flag = PSYC_NEED_LENGTH;
	else if (memmem(data, dlength, PSYC_PACKET_DELIMITER,
				sizeof(PSYC_PACKET_DELIMITER)))
	    render->flag = PSYC_NEED_LENGTH;
	else
	    render->flag = PSYC_FINE;

	// TBD
	return 0;
}
