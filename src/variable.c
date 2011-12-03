#include "lib.h"
#include <stdint.h>

#include <psyc/packet.h>


/// Routing variables in alphabetical order.
const PsycString psyc_routing_vars[] = {
    PSYC_C2STRI("_amount_fragments"),
    PSYC_C2STRI("_context"),
    //PSYC_C2STRI("_count"), // older PSYC
    PSYC_C2STRI("_counter"),
    PSYC_C2STRI("_fragment"),
    //PSYC_C2STRI("_length"), // older PSYC
    PSYC_C2STRI("_source"),
    //PSYC_C2STRI("_source_identification"), // older PSYC
    PSYC_C2STRI("_source_identity"),
    PSYC_C2STRI("_source_relay"),
    // until you have a better idea.. is this really in use?
    PSYC_C2STRI("_source_relay_relay"),
    PSYC_C2STRI("_tag"),
    PSYC_C2STRI("_tag_relay"),
    //PSYC_C2STRI("_tag_reply"), // older PSYC
    PSYC_C2STRI("_target"),
    PSYC_C2STRI("_target_forward"),
    PSYC_C2STRI("_target_relay"),
    //PSYC_C2STRI("_understand_modules"), // older PSYC
    //PSYC_C2STRI("_using_modules"), // older PSYC
};

// Variable types in alphabetical order.
const PsycDictInt psyc_var_types[] = {
    { PSYC_C2STRI("_amount"),	PSYC_TYPE_AMOUNT },
    { PSYC_C2STRI("_color"),	PSYC_TYPE_COLOR },
    { PSYC_C2STRI("_date"),	PSYC_TYPE_DATE },
    { PSYC_C2STRI("_def"),	PSYC_TYPE_DEF },
    { PSYC_C2STRI("_degree"),	PSYC_TYPE_DEGREE },
    { PSYC_C2STRI("_entity"),	PSYC_TYPE_ENTITY },
    { PSYC_C2STRI("_flag"),	PSYC_TYPE_FLAG },
    { PSYC_C2STRI("_language"),	PSYC_TYPE_LANGUAGE },
    { PSYC_C2STRI("_list"),	PSYC_TYPE_LIST },
    { PSYC_C2STRI("_nick"),	PSYC_TYPE_NICK },
    { PSYC_C2STRI("_page"),	PSYC_TYPE_PAGE },
    { PSYC_C2STRI("_table"),	PSYC_TYPE_TABLE },
    { PSYC_C2STRI("_time"),	PSYC_TYPE_TIME },
    { PSYC_C2STRI("_uniform"),	PSYC_TYPE_UNIFORM },
};

/// Method names in alphabetical order.
const PsycDictInt psyc_methods[] = {
    { PSYC_C2STRI("_data"),			PSYC_MC_DATA },
    { PSYC_C2STRI("_echo_context_enter"),	PSYC_MC_ECHO_CONTEXT_ENTER },
    { PSYC_C2STRI("_echo_context_leave"),	PSYC_MC_ECHO_CONTEXT_LEAVE },
    { PSYC_C2STRI("_echo"),			PSYC_MC_ECHO },
    { PSYC_C2STRI("_failure"),			PSYC_MC_FAILURE },
    { PSYC_C2STRI("_info"),			PSYC_MC_INFO },
    { PSYC_C2STRI("_message_action"),		PSYC_MC_MESSAGE_ACTION },
    { PSYC_C2STRI("_message_echo_action"),	PSYC_MC_MESSAGE_ECHO_ACTION },
    { PSYC_C2STRI("_message_echo"),		PSYC_MC_MESSAGE_ECHO },
    { PSYC_C2STRI("_message"),			PSYC_MC_MESSAGE },
    { PSYC_C2STRI("_notice_context_enter"),	PSYC_MC_NOTICE_CONTEXT_ENTER },
    { PSYC_C2STRI("_notice_context_leave"),	PSYC_MC_NOTICE_CONTEXT_LEAVE },
    { PSYC_C2STRI("_notice"),			PSYC_MC_NOTICE },
    { PSYC_C2STRI("_request_context_enter"),	PSYC_MC_REQUEST_CONTEXT_ENTER },
    { PSYC_C2STRI("_request_context_leave"),	PSYC_MC_REQUEST_CONTEXT_LEAVE },
    { PSYC_C2STRI("_request"),			PSYC_MC_REQUEST },
    { PSYC_C2STRI("_status_contexts_entered"),	PSYC_MC_STATUS_CONTEXTS_ENTERED },
    { PSYC_C2STRI("_status"),			PSYC_MC_STATUS },
    { PSYC_C2STRI("_warning"),			PSYC_MC_WARNING },
};

const size_t psyc_routing_vars_num = PSYC_NUM_ELEM(psyc_routing_vars);
const size_t psyc_var_types_num = PSYC_NUM_ELEM(psyc_var_types);
const size_t psyc_methods_num = PSYC_NUM_ELEM(psyc_methods);

/**
 * Get the type of variable name.
 */
inline PsycBool
psyc_var_is_routing (const char *name, size_t len)
{
    size_t cursor = 1;
    uint8_t i, m = 0;
    int8_t matching[psyc_routing_vars_num]; // indexes of matching vars

    if (len < 2 || name[0] != '_')
	return PSYC_FALSE;

    // first find the vars with matching length
    for (i = 0; i < psyc_routing_vars_num; i++)
	if (len == psyc_routing_vars[i].length)
	    matching[m++] = i;

    matching[m] = -1; // mark the end of matching indexes

    while (cursor < len && matching[0] >= 0) {
	for (i = m = 0; i < psyc_routing_vars_num; i++) {
	    if (matching[i] < 0)
		break; // reached the end of possible matches
	    if (psyc_routing_vars[matching[i]].data[cursor] == name[cursor])
		matching[m++] = matching[i]; // found a match, update matching indexes
	    else if (psyc_routing_vars[matching[i]].data[cursor] > name[cursor])
		break; // passed the possible matches in alphabetical order in the array
	}

	if (m < psyc_routing_vars_num)
	    matching[m] = -1; // mark the end of matching indexes

	cursor++;
    }

    return matching[0] >= 0 ? PSYC_TRUE : PSYC_FALSE;
}

/**
 * Get the type of variable name.
 */
inline PsycType
psyc_var_type (const char *name, size_t len)
{
    int8_t m[psyc_var_types_num];
    return (PsycType) psyc_dict_lookup((PsycDict *) psyc_var_types,
				       psyc_var_types_num, name, len, PSYC_YES,
				       (int8_t *) &m);
}

/**
 * Get the method, its family and its flags.
 */
PsycMethod
psyc_method (char *method, size_t methodlen, PsycMethod *family, unsigned int *flag)
{
    int8_t tmp[PSYC_NUM_ELEM(psyc_methods)];
    int mc = psyc_dict_lookup_int(psyc_methods, psyc_methods_num,
				  method, methodlen, PSYC_YES, tmp);

    switch (mc) {
    case PSYC_MC_DATA:
	*family = PSYC_MC_DATA;
	*flag = 0;
	break;
    case PSYC_MC_ECHO:
    case PSYC_MC_ECHO_CONTEXT_ENTER:
    case PSYC_MC_ECHO_CONTEXT_LEAVE:
	*family = PSYC_MC_ECHO;
	*flag = PSYC_METHOD_TEMPLATE | PSYC_METHOD_REPLY | PSYC_METHOD_VISIBLE;
	break;
    case PSYC_MC_ERROR:
	*family = PSYC_MC_ERROR;
	*flag = PSYC_METHOD_TEMPLATE | PSYC_METHOD_REPLY | PSYC_METHOD_VISIBLE
	    | PSYC_METHOD_LOGGABLE;
	break;
    case PSYC_MC_FAILURE:
	*family = PSYC_MC_FAILURE;
	*flag = PSYC_METHOD_TEMPLATE | PSYC_METHOD_REPLY | PSYC_METHOD_VISIBLE
	    | PSYC_METHOD_LOGGABLE;
	break;
    case PSYC_MC_INFO:
	*family = PSYC_MC_INFO;
	*flag = PSYC_METHOD_TEMPLATE | PSYC_METHOD_REPLY | PSYC_METHOD_VISIBLE
	    | PSYC_METHOD_LOGGABLE;
	break;
    case PSYC_MC_MESSAGE:
    case PSYC_MC_MESSAGE_ACTION:
	*family = PSYC_MC_MESSAGE;
	*flag = PSYC_METHOD_VISIBLE | PSYC_METHOD_LOGGABLE | PSYC_METHOD_MANUAL;
	break;
    case PSYC_MC_MESSAGE_ECHO:
    case PSYC_MC_MESSAGE_ECHO_ACTION:
	*family = PSYC_MC_MESSAGE_ECHO;
	*flag = PSYC_METHOD_REPLY | PSYC_METHOD_VISIBLE | PSYC_METHOD_LOGGABLE
	    | PSYC_METHOD_MANUAL;
	break;
    case PSYC_MC_NOTICE:
    case PSYC_MC_NOTICE_CONTEXT_ENTER:
    case PSYC_MC_NOTICE_CONTEXT_LEAVE:
	*family = PSYC_MC_NOTICE;
	*flag = PSYC_METHOD_TEMPLATE | PSYC_METHOD_VISIBLE | PSYC_METHOD_LOGGABLE;
	break;
    case PSYC_MC_REQUEST:
    case PSYC_MC_REQUEST_CONTEXT_ENTER:
    case PSYC_MC_REQUEST_CONTEXT_LEAVE:
	*family = PSYC_MC_REQUEST;
	*flag = PSYC_METHOD_TEMPLATE | PSYC_METHOD_VISIBLE | PSYC_METHOD_LOGGABLE;
	break;
    case PSYC_MC_STATUS:
    case PSYC_MC_STATUS_CONTEXTS_ENTERED:
	*family = PSYC_MC_STATUS;
	*flag = PSYC_METHOD_TEMPLATE | PSYC_METHOD_REPLY | PSYC_METHOD_VISIBLE
	    | PSYC_METHOD_LOGGABLE;
	break;
    case PSYC_MC_WARNING:
	*family = PSYC_MC_WARNING;
	*flag = PSYC_METHOD_TEMPLATE | PSYC_METHOD_REPLY | PSYC_METHOD_VISIBLE
	    | PSYC_METHOD_LOGGABLE;
	break;
    default:
	*family = mc;
	*flag = 0;
    }

    return mc;
}
