#include "psyc/lib.h"
#include "psyc/render.h"
#include "psyc/syntax.h"

inline psycRenderRC psyc_renderList(psycList *list, char *buffer, size_t buflen)
{
	size_t i, cur = 0;
	psycString *elem;

	if (list->length > buflen)
		return PSYC_RENDER_ERROR; // return error if list doesn't fit in buffer

	if (list->flag == PSYC_LIST_NEED_LENGTH)
	{
		for (i = 0; i < list->num_elems; i++)
		{
			elem = &list->elems[i];
			if (i > 0)
				buffer[cur++] = '|';
			cur += itoa(elem->length, buffer + cur, 10);
			buffer[cur++] = ' ';
			memcpy(buffer + cur, elem->ptr, elem->length);
			cur += elem->length;
		}
	}
	else
	{
		for (i = 0; i < list->num_elems; i++)
		{
			elem = &list->elems[i];
			buffer[cur++] = '|';
			memcpy(buffer + cur, elem->ptr, elem->length);
			cur += elem->length;
		}
	}

	return PSYC_RENDER_SUCCESS;
}

inline size_t psyc_renderModifier(psycModifier *mod, char *buffer)
{
	size_t cur = 0;

	buffer[cur++] = mod->oper;
	memcpy(buffer + cur, mod->name.ptr, mod->name.length);
	cur += mod->name.length;
	if (mod->flag == PSYC_MODIFIER_NEED_LENGTH)
	{
		buffer[cur++] = ' ';
		//cur += sprintf(buffer + cur, "%ld", mod->value.length);
		cur += itoa(mod->value.length, buffer + cur, 10);
	}

	buffer[cur++] = '\t';
	memcpy(buffer + cur, mod->value.ptr, mod->value.length);
	cur += mod->value.length;
	buffer[cur++] = '\n';

	return cur;
}

psycRenderRC psyc_render(psycPacket *packet, char *buffer, size_t buflen)
{
	size_t i, cur = 0;

	if (packet->length > buflen)
		return PSYC_RENDER_ERROR; // return error if packet doesn't fit in buffer

	// render routing modifiers
	for (i = 0; i < packet->routing.lines; i++)
		cur += psyc_renderModifier(&packet->routing.modifiers[i], buffer + cur);

	// add length if needed
	if (packet->flag == PSYC_PACKET_NEED_LENGTH) {
		//cur += sprintf(buffer + cur, "%ld", packet->contentLength);
		cur += itoa(packet->contentLength, buffer + cur, 10);
	}

	buffer[cur++] = '\n'; // start of content part

	// render entity modifiers
	for (i = 0; i < packet->entity.lines; i++)
		cur += psyc_renderModifier(&packet->entity.modifiers[i], buffer + cur);

	if (packet->method.length) // add method\n
	{
		memcpy(buffer + cur, packet->method.ptr, packet->method.length);
		cur += packet->method.length;
		buffer[cur++] = '\n';
	}

	if (packet->data.length) // add data\n
	{
		memcpy(buffer + cur, packet->data.ptr, packet->data.length);
		cur += packet->data.length;
		buffer[cur++] = '\n';
	}

	// add packet delimiter
	buffer[cur++] = C_GLYPH_PACKET_DELIMITER;
	buffer[cur++] = '\n';

	// actual length should be equal to pre-calculated length at this point
	assert(cur == packet->length);
	return PSYC_RENDER_SUCCESS;
}

/*
inline void psyc_initRenderState (psycRenderState *state)
{
	memset(state, 0, sizeof(psycRenderState));
}

psycRenderRC psyc_renderModifier(psycRenderState *state,
                                const char *name, size_t nlength,
                                const char *value, size_t vlength,
                                const psycRenderFlag flags, char oper)
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
	if (psyc_isRoutingVar(name, nlength))
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

psycRenderRC psyc_renderBody(psycRenderState *state,
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
