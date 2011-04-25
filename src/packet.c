#include <psyc/lib.h>
#include <psyc/syntax.h>

#include <math.h>

inline psycString psyc_newString(const char *str, size_t strlen)
{
	psycString s = {strlen, str};
	return s;
}

inline psycModifier psyc_newModifier(char oper, psycString *name, psycString *value,
                                      psycModifierFlag flag)
{
	psycModifier m = {oper, *name, *value, flag};

	if (flag == PSYC_MODIFIER_CHECK_LENGTH) // find out if it needs a length
	{
		if (value->length > PSYC_MODIFIER_SIZE_THRESHOLD)
	    m.flag = PSYC_MODIFIER_NEED_LENGTH;
		else if (memchr(value->ptr, (int)'\n', value->length))
	    m.flag = PSYC_MODIFIER_NEED_LENGTH;
		else
	    m.flag = PSYC_MODIFIER_NO_LENGTH;
	}

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

inline size_t psyc_getModifierLength(psycModifier *m)
{
	size_t length = 1 + // oper
		m->name.length + 1 + // name\t
		m->value.length + 1; // value\n

	if (m->flag == PSYC_MODIFIER_NEED_LENGTH) // add length of length if needed
		length += log10((double)m->value.length) + 2; // SP length

	return length;
}

inline psycPacket psyc_newPacket(psycModifierArray *routing,
                                  psycModifierArray *entity,
                                  psycString *method, psycString *data,
                                  psycPacketFlag flag)
{
	psycPacket p = {*routing, *entity, *method, *data, 0, 0, flag};
	size_t i;

	if (flag == PSYC_PACKET_CHECK_LENGTH) // find out if it needs a length
	{
		if (data->length == 1 && data->ptr[0] == C_GLYPH_PACKET_DELIMITER)
	    p.flag = PSYC_PACKET_NEED_LENGTH;
		else if (data->length > PSYC_CONTENT_SIZE_THRESHOLD)
	    p.flag = PSYC_PACKET_NEED_LENGTH;
		else if (memmem(data->ptr, data->length, PSYC_C2ARG(PSYC_PACKET_DELIMITER)))
	    p.flag = PSYC_PACKET_NEED_LENGTH;
		else
	    p.flag = PSYC_PACKET_NO_LENGTH;
	}

	// add routing header length
	for (i = 0; i < routing->lines; i++)
		p.routingLength += psyc_getModifierLength(&routing->modifiers[i]);

	// add entity header length
	for (i = 0; i < entity->lines; i++)
		p.contentLength += psyc_getModifierLength(&entity->modifiers[i]);

	// add length of method, data & delimiter
	if (method->length)
		p.contentLength += method->length + 1; // method\n
	if (data->length)
		p.contentLength += data->length + 1; // data\n

	// set total length: routing-header \n content |\n
	p.length = p.routingLength + 1 + p.contentLength + sizeof(PSYC_PACKET_DELIMITER) - 2;
	if (flag == PSYC_PACKET_NEED_LENGTH) // add length of length if needed
		p.length += log10((double)data->length) + 1;

	return p;
}

inline psycPacket psyc_newPacket2(psycModifier *routing, size_t routinglen,
                                   psycModifier *entity, size_t entitylen,
                                   const char *method, size_t methodlen,
                                   const char *data, size_t datalen,
                                   psycPacketFlag flag)
{
	psycModifierArray r = {routinglen, routing};
	psycModifierArray e = {entitylen, entity};
	psycString m = {methodlen, method};
	psycString d = {datalen, data};

	return psyc_newPacket(&r, &e, &m, &d, flag);
}
