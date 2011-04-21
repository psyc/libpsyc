#include "psyc/lib.h"
#include "psyc/render.h"
#include "psyc/syntax.h"

/* render PSYC packets */

int PSYC_renderBody(PSYC_RenderState* render,
                    const uint8_t* method, const size_t mlength,
                    const uint8_t* data, const size_t dlength,
                    const uint8_t flags) {

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
