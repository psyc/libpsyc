#include "psyc/lib.h"
#include "psyc/render.h"
#include "psyc/syntax.h"

psycRenderRC psyc_renderList (psycList *list, char *buffer, size_t buflen)
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

static inline
size_t psyc_renderModifier (psycModifier *mod, char *buffer)
{
	size_t cur = 0;

	buffer[cur++] = mod->oper;
	memcpy(buffer + cur, mod->name.ptr, mod->name.length);
	cur += mod->name.length;
	if (cur <= 1)
		return cur; // error, name can't be empty

	if (mod->flag == PSYC_MODIFIER_NEED_LENGTH)
	{
		buffer[cur++] = ' ';
		cur += itoa(mod->value.length, buffer + cur, 10);
	}

	buffer[cur++] = '\t';
	memcpy(buffer + cur, mod->value.ptr, mod->value.length);
	cur += mod->value.length;
	buffer[cur++] = '\n';

	return cur;
}

psycRenderRC psyc_render (psycPacket *packet, char *buffer, size_t buflen)
{
	size_t i, cur = 0, len;

	if (packet->length > buflen)
		return PSYC_RENDER_ERROR; // return error if packet doesn't fit in buffer

	// render routing modifiers
	for (i = 0; i < packet->routing.lines; i++)
	{
		len = psyc_renderModifier(&packet->routing.modifiers[i], buffer + cur);
		cur += len;
		if (len <= 1)
			return PSYC_RENDER_ERROR_MODIFIER_NAME_MISSING;
	}

	// add length if needed
	if (packet->flag == PSYC_PACKET_NEED_LENGTH)
		cur += itoa(packet->contentLength, buffer + cur, 10);

	if (packet->flag == PSYC_PACKET_NEED_LENGTH || packet->content.length ||
	    packet->entity.lines || packet->method.length || packet->data.length)
		buffer[cur++] = '\n'; // start of content part if there's content or length

	if (packet->content.length) // render raw content if present
	{
		memcpy(buffer + cur, packet->content.ptr, packet->content.length);
		cur += packet->content.length;
	}
	else
	{
		// render entity modifiers
		for (i = 0; i < packet->entity.lines; i++)
			cur += psyc_renderModifier(&packet->entity.modifiers[i], buffer + cur);

		if (packet->method.length) // add method\n
		{
			memcpy(buffer + cur, packet->method.ptr, packet->method.length);
			cur += packet->method.length;
			buffer[cur++] = '\n';

			if (packet->data.length) // add data\n
			{
				memcpy(buffer + cur, packet->data.ptr, packet->data.length);
				cur += packet->data.length;
				buffer[cur++] = '\n';
			}
		}
		else if (packet->data.length) // error, we have data but no modifier
			return PSYC_RENDER_ERROR_METHOD_MISSING;
	}

	// add packet delimiter
	buffer[cur++] = C_GLYPH_PACKET_DELIMITER;
	buffer[cur++] = '\n';

	// actual length should be equal to pre-calculated length at this point
	assert(cur == packet->length);
	return PSYC_RENDER_SUCCESS;
}
