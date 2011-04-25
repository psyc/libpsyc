#include <psyc/lib.h>
#include <psyc/syntax.h>

#include <math.h>

inline PSYC_String PSYC_newString (const char *str, size_t strlen)
{
	PSYC_String s = {strlen, str};
	return s;
}

inline PSYC_Modifier PSYC_newModifier(char oper, PSYC_String *name, PSYC_String *value,
                                      PSYC_ModifierFlag flag)
{
	PSYC_Modifier m = {oper, *name, *value, flag};

	if (flag == PSYC_MODIFIER_CHECK_LENGTH) // find out if it needs a length
	{
		if (value->length > PSYC_MODIFIER_SIZE_THRESHOLD)
	    m.flag = PSYC_PACKET_NEED_LENGTH;
		else if (memchr(value->ptr, (int)'\n', value->length))
	    m.flag = PSYC_PACKET_NEED_LENGTH;
		else
	    m.flag = PSYC_PACKET_NO_LENGTH;
	}

	return m;
}

inline PSYC_Modifier PSYC_newModifier2(char oper,
                                       const char *name, size_t namelen,
                                       const char *value, size_t valuelen,
                                       PSYC_ModifierFlag flag)
{
	PSYC_String n = {namelen, name};
	PSYC_String v = {valuelen, value};

	return PSYC_newModifier(oper, &n, &v, flag);
}

inline size_t PSYC_getModifierLength(PSYC_Modifier *m)
{
	size_t length =
		m->name.length + 1 + // name\t
		m->value.length + 1; // value\n

	if (m->flag == PSYC_MODIFIER_NEED_LENGTH) // add length of length if needed
		length += log10((double)m->value.length) + 2; // SP length

	return length;
}

inline PSYC_Packet PSYC_newPacket(PSYC_ModifierArray *routing,
                                  PSYC_ModifierArray *entity,
                                  PSYC_String *method, PSYC_String *data,
                                  PSYC_PacketFlag flag)
{
	PSYC_Packet p = {*routing, *entity, *method, *data, 0, 0, flag};
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

	// calculate routing header length
	for (i = 0; i < routing->lines; i++)
		p.routingLength += PSYC_getModifierLength(routing->modifiers[i]);

	// calculate entity header length
	for (i = 0; i < entity->lines; i++)
		p.contentLength += PSYC_getModifierLength(routing->modifiers[i]);

	// add length of method, data & delimiter
	p.contentLength += method->length + 1 + data->length; // method \n data

	// set total length: routing-header \n content \n|\n
	p.length = p.routingLength + 1 + p.contentLength + sizeof(PSYC_PACKET_DELIMITER) - 1; 
	if (flag == PSYC_PACKET_NEED_LENGTH) // add length of length if needed
		p.length += log10((double)data->length) + 1;	

	return p;
}

inline PSYC_Packet PSYC_newPacket2(PSYC_Modifier **routing, size_t routinglen,
                                   PSYC_Modifier **entity, size_t entitylen,
                                   const char *method, size_t methodlen,
                                   const char *data, size_t datalen,
                                   PSYC_PacketFlag flag)
{
	PSYC_ModifierArray r = {routinglen, routing};
	PSYC_ModifierArray e = {entitylen, entity};
	PSYC_String m = {methodlen, method};
	PSYC_String d = {datalen, data};

	return PSYC_newPacket(&r, &e, &m, &d, flag);
}
