#include <psyc/lib.h>
#include <psyc/syntax.h>

#include <math.h>

inline psycString psyc_newString(const char *str, size_t strlen)
{
	psycString s = {strlen, str};
	return s;
}

inline psycModifierFlag psyc_checkModifierLength(psycModifier *m)
{
	psycModifierFlag flag;

	if (m->value.length > PSYC_MODIFIER_SIZE_THRESHOLD)
		flag = PSYC_MODIFIER_NEED_LENGTH;
	else if (memchr(m->value.ptr, (int)'\n', m->value.length))
		flag = PSYC_MODIFIER_NEED_LENGTH;
	else
		flag = PSYC_MODIFIER_NO_LENGTH;

	return flag;
}

inline size_t psyc_getModifierLength(psycModifier *m)
{
	size_t length = 1 + // oper
		m->name.length + 1 + // name\t
		m->value.length + 1; // value\n

	if (m->flag == PSYC_MODIFIER_NEED_LENGTH) // add length of length if needed
		length += log10((double)m->value.length) + 2; // SP length

	return length;
}

inline psycModifier psyc_newModifier(char oper, psycString *name, psycString *value,
                                     psycModifierFlag flag)
{
	psycModifier m = {oper, *name, *value, flag};

	if (flag == PSYC_MODIFIER_CHECK_LENGTH) // find out if it needs a length
		m.flag = psyc_checkModifierLength(&m);

	return m;
}

inline psycModifier psyc_newModifier2(char oper,
                                      const char *name, size_t namelen,
                                      const char *value, size_t valuelen,
                                      psycModifierFlag flag)
{
	psycString n = {namelen, name};
	psycString v = {valuelen, value};

	return psyc_newModifier(oper, &n, &v, flag);
}

inline psycListFlag psyc_checkListLength(psycList *list)
{
	psycListFlag flag = PSYC_LIST_NO_LENGTH;
	size_t i, length = 0;

	for (i = 0; i < list->num_elems; i++)
	{
		psycString *elem = &list->elems[i];
		length += 1 + elem->length; // |elem
		if (length > PSYC_MODIFIER_SIZE_THRESHOLD ||
				memchr(elem->ptr, (int)'|', elem->length) ||
				memchr(elem->ptr, (int)'\n', elem->length))
		{
			flag = PSYC_LIST_NEED_LENGTH;
			break;
		}
	}

	return flag;
}

inline psycListFlag psyc_getListLength(psycList *list)
{
	size_t i, length = 0;

	if (list->flag == PSYC_LIST_NEED_LENGTH)
	{
		for (i = 0; i < list->num_elems; i++)
		{
			if (i > 0)
				length++; // |
			length += log10((double)list->elems[i].length) + 2 + list->elems[i].length; // length SP elem
		}
	}
	else
	{
		for (i = 0; i < list->num_elems; i++)
			length += 1 + list->elems[i].length; // |elem
	}

	return length;
}

inline psycList psyc_newList(psycString *elems, size_t num_elems, psycListFlag flag)
{
	psycList list = {num_elems, elems, 0, flag};

	if (flag == PSYC_LIST_CHECK_LENGTH) // check if list elements need length
		list.flag = psyc_checkListLength(&list);

	list.length = psyc_getListLength(&list);
	return list;
}

inline psycPacketFlag psyc_checkPacketLength(psycPacket *p)
{
	if (p->data.length == 1 && p->data.ptr[0] == C_GLYPH_PACKET_DELIMITER)
		return PSYC_PACKET_NEED_LENGTH;

	if (p->data.length > PSYC_CONTENT_SIZE_THRESHOLD)
		return PSYC_PACKET_NEED_LENGTH;

	int i;
	// if any entity modifiers need length it is possible they contain
	// a packet terminator, thus the content should have a length as well
	for (i = 0; i < p->entity.lines; i++)
		if (p->entity.modifiers[i].flag == PSYC_MODIFIER_NEED_LENGTH)
			return PSYC_PACKET_NEED_LENGTH;

	if (memmem(p->data.ptr, p->data.length, PSYC_C2ARG(PSYC_PACKET_DELIMITER)))
		return PSYC_PACKET_NEED_LENGTH;

	return PSYC_PACKET_NO_LENGTH;
}

inline size_t psyc_setPacketLength(psycPacket *p)
{
	size_t i;
	p->routingLength = 0;
	p->contentLength = 0;

	// add routing header length
	for (i = 0; i < p->routing.lines; i++)
		p->routingLength += psyc_getModifierLength(&(p->routing.modifiers[i]));

	// add entity header length
	for (i = 0; i < p->entity.lines; i++)
		p->contentLength += psyc_getModifierLength(&(p->entity.modifiers[i]));

	// add length of method, data & delimiter
	if (p->method.length)
		p->contentLength += p->method.length + 1; // method\n
	if (p->data.length)
		p->contentLength += p->data.length + 1; // data\n

	// set total length: routing-header content |\n
	p->length = p->routingLength + p->contentLength + 2;

	if (p->contentLength > 0)
	{
		p->length++; // add \n at the start of the content part
		if (p->flag == PSYC_PACKET_NEED_LENGTH) // add length of length if needed
			p->length += log10((double)p->contentLength) + 1;
	}

	return p->length;
}

inline psycPacket psyc_newPacket(psycHeader *routing,
                                 psycHeader *entity,
                                 psycString *method, psycString *data,
                                 psycPacketFlag flag)
{
	psycPacket p = {*routing, *entity, *method, *data, 0, 0, flag};

	if (flag == PSYC_PACKET_CHECK_LENGTH) // find out if it needs a length
		p.flag = psyc_checkPacketLength(&p);

	psyc_setPacketLength(&p);
	return p;
}

inline psycPacket psyc_newPacket2(psycModifier *routing, size_t routinglen,
                                  psycModifier *entity, size_t entitylen,
                                  const char *method, size_t methodlen,
                                  const char *data, size_t datalen,
                                  psycPacketFlag flag)
{
	psycHeader r = {routinglen, routing};
	psycHeader e = {entitylen, entity};
	psycString m = {methodlen, method};
	psycString d = {datalen, data};

	return psyc_newPacket(&r, &e, &m, &d, flag);
}
