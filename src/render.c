#include "psyc/lib.h"
#include "psyc/render.h"

inline int PSYC_renderModifier(PSYC_Modifier *m, char *buffer)
{
	int cur = 0;

	buffer[cur++] = m->oper;
	memcpy(buffer + cur, m->name.ptr, m->name.length);
	cur += m->name.length;
	if (m->flag == PSYC_MODIFIER_NEED_LENGTH)
	{
		buffer[cur++] = ' ';
//		cur += sprintf(buffer + cur, "%ld", m->value.length);
		cur += itoa(m->value.length, buffer + cur, 10);
	}
	memcpy(buffer + cur, m->value.ptr, m->value.length);
	cur += m->value.length;
	buffer[cur++] = '\n';

	return cur;
}

PSYC_RenderRC PSYC_render(PSYC_Packet *packet, char *buffer, size_t buflen)
{
	size_t cur, i;

	if (packet->length > buflen)
		return PSYC_RENDER_ERROR;

	for (i = 0; i < packet->routing.lines; i++)
		cur += PSYC_renderModifier(packet->routing.modifiers[i], buffer + cur);

	if (packet->flag == PSYC_PACKET_NEED_LENGTH) {
//		cur += sprintf(buffer + cur, "%ld", packet->contentLength);
		cur += itoa(packet->contentLength, buffer + cur, 10);
	}

	buffer[cur++] = '\n';

	for (i = 0; i < packet->entity.lines; i++)
		cur += PSYC_renderModifier(packet->entity.modifiers[i], buffer + cur);

	memcpy(buffer + cur, PSYC_C2ARG(PSYC_PACKET_DELIMITER));

	return PSYC_RENDER_SUCCESS;
}

/*
inline void PSYC_initRenderState (PSYC_RenderState *state)
{
	memset(state, 0, sizeof(PSYC_RenderState));
}

PSYC_RenderRC PSYC_renderModifier(PSYC_RenderState *state,
                                const char *name, size_t nlength,
                                const char *value, size_t vlength,
                                const PSYC_RenderFlag flags, char oper)
{
	size_t startc = state->cursor;

	unless (nlength) nlength = strlen(name);
	// vlength 0 means an empty variable.. no cheating there
	unless (oper) oper = C_GLYPH_OPERATOR_SET;

	state->buffer[state->cursor++] = oper;
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

PSYC_RenderRC PSYC_renderBody(PSYC_RenderState *state,
                    const char *method, size_t mlength,
                    const char *data, size_t dlength)
{
	if (state->flag == PSYC_RENDER_CHECK_LENGTH)
	{
		// find out if this packet needs a prepended length
		if (dlength == 1 && data[0] == C_GLYPH_PACKET_DELIMITER)
	    state->flag = PSYC_RENDER_NEED_LENGTH;
		else if (dlength > 404)
	    state->flag = PSYC_RENDER_NEED_LENGTH;
		else if (memmem(data, dlength, PSYC_C2ARG(PSYC_PACKET_DELIMITER)))
	    state->flag = PSYC_RENDER_NEED_LENGTH;
		else
	    state->flag = PSYC_RENDER_NO_LENGTH;
	}

	// TBD

	return PSYC_RENDER_SUCCESS;
}
*/
