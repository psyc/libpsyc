#include "lib.h"
#include <psyc/packet.h>

inline PsycElemFlag
psyc_elem_length_check (PsycString *value, const char end)
{
    if (value->length > PSYC_ELEM_SIZE_THRESHOLD
	|| memchr(value->data, (int)end, value->length))
	return PSYC_ELEM_NEED_LENGTH;

    return PSYC_ELEM_NO_LENGTH;;
}

inline size_t
psyc_elem_length (PsycElem *elem)
{
    return
	(elem->type.length ? 1 + elem->type.length : 0)
	+ (elem->value.length && elem->flag != PSYC_ELEM_NO_LENGTH
	   ? (elem->type.length ? 1 : 0) + psyc_num_length(elem->value.length) : 0)
	+ (elem->value.length ? 1 + elem->value.length : 0);
}

inline size_t
psyc_dict_key_length (PsycDictKey *elem)
{
    return
	(elem->flag != PSYC_ELEM_NO_LENGTH
	 ? psyc_num_length(elem->value.length) : 0)
	+ (elem->value.length ? 1 + elem->value.length : 0);
}

inline size_t
psyc_list_length_set (PsycList *list)
{
    size_t i;
    PsycElem *elem;

    list->length = list->type.length;

    for (i = 0; i < list->num_elems; i++) {
	elem = &list->elems[i];
	if (elem->flag == PSYC_ELEM_CHECK_LENGTH)
	    elem->flag = psyc_elem_length_check(&elem->value, '|');
	elem->length = psyc_elem_length(elem);
	list->length += 1 + elem->length;
    }

    return list->length;
}

inline size_t
psyc_dict_length_set (PsycDict *dict)
{
    size_t i;
    PsycDictKey *key;
    PsycElem *value;

    dict->length = dict->type.length;

    for (i = 0; i < dict->num_elems; i++) {
	key = &dict->elems[i].key;
	value = &dict->elems[i].value;

	if (key->flag == PSYC_ELEM_CHECK_LENGTH)
	    key->flag = psyc_elem_length_check(&key->value, '}');
	if (value->flag == PSYC_ELEM_CHECK_LENGTH)
	    value->flag = psyc_elem_length_check(&value->value, '{');

	key->length = psyc_dict_key_length(key);
	value->length = psyc_elem_length(value);

	dict->length += 1 + key->length + 1 + value->length;
    }

    return dict->length;
}

void
psyc_list_init (PsycList *list, PsycElem *elems, size_t num_elems)
{
    *list = (PsycList) {
	.num_elems = num_elems,
	.elems = elems,
    };
    psyc_list_length_set(list);
}

void
psyc_dict_init (PsycDict *dict, PsycDictElem *elems, size_t num_elems)
{
    *dict = (PsycDict) {
	.num_elems = num_elems,
	.elems = elems,
    };
    psyc_dict_length_set(dict);
}

inline size_t
psyc_modifier_length (PsycModifier *m)
{
    size_t length = 2; // oper\n
    if (m->name.length > 0)
	length += m->name.length + 1 + m->value.length; // name\tvalue

    // add length of length if needed
    if (m->value.length
	&& (m->flag & PSYC_MODIFIER_NEED_LENGTH
	    || m->flag == PSYC_MODIFIER_CHECK_LENGTH))
	length += psyc_num_length(m->value.length) + 1; // SP length

    return length;
}

inline PsycPacketFlag
psyc_packet_length_check (PsycPacket *p)
{
    if (p->data.length == 1 && p->data.data[0] == PSYC_PACKET_DELIMITER_CHAR)
	return PSYC_PACKET_NEED_LENGTH;

    if (p->data.length > PSYC_CONTENT_SIZE_THRESHOLD)
	return PSYC_PACKET_NEED_LENGTH;

    int i;
    // If any entity modifiers need length, it is possible they contain
    // a packet terminator, thus the content should have a length as well.
    for (i = 0; i < p->entity.lines; i++)
	if (p->entity.modifiers[i].flag & PSYC_MODIFIER_NEED_LENGTH
	    || p->entity.modifiers[i].flag == PSYC_MODIFIER_CHECK_LENGTH)
	    return PSYC_PACKET_NEED_LENGTH;

    if (memmem(p->data.data, p->data.length, PSYC_C2ARG(PSYC_PACKET_DELIMITER)))
	return PSYC_PACKET_NEED_LENGTH;

    return PSYC_PACKET_NO_LENGTH;
}

inline size_t
psyc_packet_length_set (PsycPacket *p)
{
    size_t i;
    p->routinglen = 0;
    p->contentlen = 0;

    // add routing header length
    for (i = 0; i < p->routing.lines; i++)
	p->routinglen += psyc_modifier_length(&(p->routing.modifiers[i]));

    if (p->content.length)
	p->contentlen = p->content.length;
    else {
	// add state operation
	if (p->stateop != PSYC_STATE_NOOP)
	    p->contentlen += 2;	// op\n

	// add entity header length
	for (i = 0; i < p->entity.lines; i++)
	    p->contentlen += psyc_modifier_length(&(p->entity.modifiers[i]));

	// add length of method, data & delimiter
	if (p->method.length)
	    p->contentlen += p->method.length + 1;	// method\n
	if (p->data.length)
	    p->contentlen += p->data.length + 1;	// data\n
    }

    // set total length: routing-header content |\n
    p->length = p->routinglen + p->contentlen + 2;

    if (p->contentlen)
	p->length++; // add \n at the start of the content part

    // add length of length if needed
    if (p->contentlen && !(p->flag & PSYC_PACKET_NO_LENGTH))
	p->length += psyc_num_length(p->contentlen);

    return p->length;
}

inline void
psyc_packet_init (PsycPacket *p,
		  PsycModifier *routing, size_t routinglen,
		  PsycModifier *entity, size_t entitylen,
		  char *method, size_t methodlen,
		  char *data, size_t datalen,
		  char stateop, PsycPacketFlag flag)
{
    *p = (PsycPacket) {
	.routing = {routinglen, routing},
	.entity = {entitylen, entity},
	.method = {methodlen, method},
	.data = {datalen, data},
	.content = {0, 0},
	.routinglen = 0,
	.contentlen = 0,
	.length = 0,
	.stateop = stateop,
	.flag = flag,
    };

    if (flag == PSYC_PACKET_CHECK_LENGTH) // find out if it needs length
	p->flag = psyc_packet_length_check(p);

    psyc_packet_length_set(p);
}

inline void
psyc_packet_init_raw (PsycPacket *p,
		      PsycModifier *routing, size_t routinglen,
		      char *content, size_t contentlen,
		      PsycPacketFlag flag)
{
    *p = (PsycPacket) {
	.routing = {routinglen, routing},
	.entity = {0, 0},
	.method = {0, 0},
	.data = {0, 0},
	.content = {contentlen, content},
	.routinglen = 0,
	.contentlen = 0,
	.length = 0,
	.stateop = 0,
	.flag = flag,
    };

    if (flag == PSYC_PACKET_CHECK_LENGTH) // find out if it needs length
	p->flag = psyc_packet_length_check(p);

    psyc_packet_length_set(p);
}

void
psyc_packet_id (PsycList *list, PsycElem *elems,
		char *context, size_t contextlen,
		char *source, size_t sourcelen,
		char *target, size_t targetlen,
		char *counter, size_t counterlen,
		char *fragment, size_t fragmentlen)
{
    if (contextlen)
	elems[PSYC_PACKET_ID_CONTEXT] =
	    PSYC_ELEM_VF(context, contextlen, PSYC_ELEM_NO_LENGTH);
    if (sourcelen)
	elems[PSYC_PACKET_ID_SOURCE] =
	    PSYC_ELEM_VF(source, sourcelen, PSYC_ELEM_NO_LENGTH);
    if (targetlen)
	elems[PSYC_PACKET_ID_TARGET] =
	    PSYC_ELEM_VF(target, targetlen, PSYC_ELEM_NO_LENGTH);
    if (counterlen)
	elems[PSYC_PACKET_ID_COUNTER] =
	    PSYC_ELEM_VF(counter, counterlen, PSYC_ELEM_NO_LENGTH);
    if (fragmentlen)
	elems[PSYC_PACKET_ID_FRAGMENT] =
	    PSYC_ELEM_VF(fragment, fragmentlen, PSYC_ELEM_NO_LENGTH);

    psyc_list_init(list, elems, PSYC_PACKET_ID_ELEMS);
}
