/*
  This file is part of libpsyc.
  Copyright (C) 2011,2012 Carlo v. Loesch, Gabor X Toth, Mathias L. Baumann,
  and other contributing authors.

  libpsyc is free software: you can redistribute it and/or modify it under the
  terms of the GNU Affero General Public License as published by the Free
  Software Foundation, either version 3 of the License, or (at your option) any
  later version. As a special exception, libpsyc is distributed with additional
  permissions to link libpsyc libraries with non-AGPL works.

  libpsyc is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
  details.

  You should have received a copy of the GNU Affero General Public License and
  the linking exception along with libpsyc in a COPYING file.
*/

#include <stdio.h>

#include "lib.h"
#include <psyc/packet.h>
#include <psyc/render.h>

inline PsycRenderRC
psyc_render_elem (PsycElem *elem, char *buffer, size_t buflen)
{
    size_t cur = 0;

    if (elem->length > buflen) // return error if element doesn't fit in buffer
	return PSYC_RENDER_ERROR;

    if (elem->type.length) {
	buffer[cur++] = '=';
	memcpy(buffer + cur, PSYC_S2ARG(elem->type));
	cur += elem->type.length;
    }

    if (elem->value.length && !(elem->flag & PSYC_ELEM_NO_LENGTH)) {
	if (elem->type.length)
	    buffer[cur++] = ':';
	cur += itoa(elem->value.length, buffer + cur, 10);
    }

    if (elem->value.length) {
	buffer[cur++] = ' ';
	memcpy(buffer + cur, PSYC_S2ARG(elem->value));
	cur += elem->value.length;
    }

    // Actual length should be equal to pre-calculated length at this point.
    ASSERT(cur == elem->length);
    return PSYC_RENDER_SUCCESS;
}

inline PsycRenderRC
psyc_render_dict_key (PsycDictKey *elem, char *buffer, size_t buflen)
{
    size_t cur = 0;

    if (elem->length > buflen) // return error if element doesn't fit in buffer
	return PSYC_RENDER_ERROR;

    if (elem->value.length && !(elem->flag & PSYC_ELEM_NO_LENGTH))
	cur += itoa(elem->value.length, buffer + cur, 10);

    if (elem->value.length) {
	buffer[cur++] = ' ';
	memcpy(buffer + cur, PSYC_S2ARG(elem->value));
	cur += elem->value.length;
    }

    // Actual length should be equal to pre-calculated length at this point.
    ASSERT(cur == elem->length);
    return PSYC_RENDER_SUCCESS;
}

#ifdef __INLINE_PSYC_RENDER
extern inline
#endif
PsycRenderRC
psyc_render_list (PsycList *list, char *buffer, size_t buflen)
{
    size_t i, cur = 0;

    ASSERT(NULL != list);
    if (list->length > buflen) // return error if list doesn't fit in buffer
	return PSYC_RENDER_ERROR;

    ASSERT(NULL != buffer);
    if (list->type.length) {
	memcpy(buffer + cur, PSYC_S2ARG(list->type));
	cur += list->type.length;
    }

    for (i = 0; i < list->num_elems; i++) {
	buffer[cur++] = '|';
	psyc_render_elem(&list->elems[i], buffer + cur, buflen - cur);
	cur += list->elems[i].length;
    }

    // Actual length should be equal to pre-calculated length at this point.
    ASSERT(cur == list->length);
    return PSYC_RENDER_SUCCESS;
}

#ifdef __INLINE_PSYC_RENDER
extern inline
#endif
PsycRenderRC
psyc_render_dict (PsycDict *dict, char *buffer, size_t buflen)
{
    size_t i, cur = 0;

    if (dict->length > buflen) // return error if dict doesn't fit in buffer
	return PSYC_RENDER_ERROR;

    if (dict->type.length) {
	memcpy(buffer + cur, PSYC_S2ARG(dict->type));
	cur += dict->type.length;
    }

    for (i = 0; i < dict->num_elems; i++) {
	buffer[cur++] = '{';
	psyc_render_dict_key(&dict->elems[i].key, buffer + cur, buflen - cur);
	cur += dict->elems[i].key.length;

	buffer[cur++] = '}';
	psyc_render_elem(&dict->elems[i].value, buffer + cur, buflen - cur);
	cur += dict->elems[i].value.length;
    }

    // Actual length should be equal to pre-calculated length at this point.
    ASSERT(cur == dict->length);
    return PSYC_RENDER_SUCCESS;
}

inline size_t
psyc_render_modifier (PsycModifier *mod, char *buffer)
{
    size_t cur = 0;

    buffer[cur++] = mod->oper;
    memcpy(buffer + cur, mod->name.data, mod->name.length);
    cur += mod->name.length;
    if (cur == 1)
	return cur; // error, name can't be empty

    if (mod->value.length
	&& (mod->flag & PSYC_MODIFIER_NEED_LENGTH
	    || mod->flag == PSYC_MODIFIER_CHECK_LENGTH)) {
	buffer[cur++] = ' ';
	cur += itoa(mod->value.length, buffer + cur, 10);
    }

    buffer[cur++] = '\t';
    memcpy(buffer + cur, mod->value.data, mod->value.length);
    cur += mod->value.length;
    buffer[cur++] = '\n';

    return cur;
}

#ifdef __INLINE_PSYC_RENDER
extern inline
#endif
PsycRenderRC
psyc_render (PsycPacket *p, char *buffer, size_t buflen)
{
    size_t i, cur = 0, len;

    if (p->length > buflen) // return error if packet doesn't fit in buffer
	return PSYC_RENDER_ERROR;

    // render routing modifiers
    for (i = 0; i < p->routing.lines; i++) {
	len = psyc_render_modifier(&p->routing.modifiers[i], buffer + cur);
	cur += len;
	if (len <= 1)
	    return PSYC_RENDER_ERROR_MODIFIER_NAME_MISSING;
    }

    // add length if needed
    if (p->contentlen && !(p->flag & PSYC_PACKET_NO_LENGTH))
	cur += itoa(p->contentlen, buffer + cur, 10);

    if (p->contentlen)
	buffer[cur++] = '\n'; // start of content part if there's content or length

    if (p->content.length) { // render raw content if present
	memcpy(buffer + cur, p->content.data, p->content.length);
	cur += p->content.length;
    } else {
	if (p->stateop) {
	    buffer[cur++] = p->stateop;
	    buffer[cur++] = '\n';
	}
	// render entity modifiers
	for (i = 0; i < p->entity.lines; i++)
	    cur += psyc_render_modifier(&p->entity.modifiers[i],
					buffer + cur);

	if (p->method.length) { // add method\n
	    memcpy(buffer + cur, p->method.data, p->method.length);
	    cur += p->method.length;
	    buffer[cur++] = '\n';

	    if (p->data.length) { // add data\n
		memcpy(buffer + cur, p->data.data, p->data.length);
		cur += p->data.length;
		buffer[cur++] = '\n';
	    }
	} else if (p->data.length)	// error, we have data but no modifier
	    return PSYC_RENDER_ERROR_METHOD_MISSING;
    }

    // add packet delimiter
    buffer[cur++] = PSYC_PACKET_DELIMITER_CHAR;
    buffer[cur++] = '\n';

    // actual length should be equal to pre-calculated length at this point
    ASSERT(cur == p->length);
    return PSYC_RENDER_SUCCESS;
}
