#include "psyc/lib.h"
#include "psyc/render.h"

PSYC_RenderRC PSYC_renderVar(PSYC_RenderState* state,
                                const char* name, size_t nlength,
                                const char* value, size_t vlength,
                                const PSYC_RenderFlag flags, char modifier)
{
	size_t startc = state->cursor;

	unless (nlength) nlength = strlen(name);
	// vlength 0 means an empty variable.. no cheating there
	unless (modifier) modifier = C_GLYPH_MODIFIER_SET;

	state->buffer[state->cursor++] = modifier;
	strncpy(&state->buffer[state->cursor], name, nlength);
	state->cursor += nlength;

	if (vlength)
	{
		state->buffer[state->cursor++] = '\t';
		strncpy(&state->buffer[state->cursor], value, vlength);
		state->cursor += vlength;
	}

	//if (flags == PSYC_RENDER_ROUTING)
	if (PSYC_isRoutingVar(name, nlength))
	{ // no more routing headers allowed after content started
		if (state->part != PSYC_PART_ROUTING)
		{
			P1(("Too late to add a routing variable!\n"));
			return PSYC_RENDER_ERROR_ROUTING;
		}
	}
	else if (state->part == PSYC_PART_ROUTING)
	{ // first entity header, set part to content
		state->part = PSYC_PART_CONTENT;
		// add "\n000000000" to buffer
		// and make spot point to the first 0
	}

	// update content length if we're in the content part
	if (state->part == PSYC_PART_CONTENT)
		state->contentLength += state->cursor - startc;

	return PSYC_RENDER_SUCCESS;
}

/* render PSYC packets */
PSYC_RenderRC PSYC_renderBody(PSYC_RenderState* state,
                    const char* method, size_t mlength,
                    const char* data, size_t dlength)
{
	if (state->flag == PSYC_RENDER_CHECK_LENGTH)
	{
		// find out if this packet needs a prepended length
		if (dlength == 1 && data[0] == C_GLYPH_PACKET_DELIMITER)
	    state->flag = PSYC_RENDER_NEED_LENGTH;
		else if (dlength > 404)
	    state->flag = PSYC_RENDER_NEED_LENGTH;
		else if (memmem(data, dlength, PSYC_PACKET_DELIMITER, sizeof(PSYC_PACKET_DELIMITER)))
	    state->flag = PSYC_RENDER_NEED_LENGTH;
		else
	    state->flag = PSYC_RENDER_NO_LENGTH;
	}

	// TBD

	return PSYC_RENDER_SUCCESS;
}
