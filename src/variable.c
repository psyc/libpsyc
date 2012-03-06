#include "lib.h"
#include <stdint.h>

#include <psyc/packet.h>

/// Routing variables in alphabetical order.
const PsycMapInt psyc_rvars[] = {
    { PSYC_C2STRI("_amount_fragments"),	PSYC_RVAR_AMOUNT_FRAGMENTS },
    { PSYC_C2STRI("_context"),		PSYC_RVAR_CONTEXT },
    { PSYC_C2STRI("_counter"),		PSYC_RVAR_COUNTER },
    { PSYC_C2STRI("_fragment"),		PSYC_RVAR_FRAGMENT },
    { PSYC_C2STRI("_source"),		PSYC_RVAR_SOURCE },
    { PSYC_C2STRI("_source_relay"),	PSYC_RVAR_SOURCE_RELAY },
    { PSYC_C2STRI("_tag"),		PSYC_RVAR_TAG },
    { PSYC_C2STRI("_tag_relay"),	PSYC_RVAR_TAG_RELAY },
    { PSYC_C2STRI("_target"),		PSYC_RVAR_TARGET },
    { PSYC_C2STRI("_target_relay"),	PSYC_RVAR_TARGET_RELAY },

/* old psyc:
    { PSYC_C2STRI("_length"),			PSYC_RVAR_LENGTH },
    { PSYC_C2STRI("_source_identification"),	PSYC_RVAR_SOURCE_IDENTIFICATION },
    { PSYC_C2STRI("_source_identity"),		PSYC_RVAR_SOURCE_IDENTITY },
    { PSYC_C2STRI("_source_relay_relay"),	PSYC_RVAR_RELAY_RELAY },
    { PSYC_C2STRI("_tag_reply"),		PSYC_RVAR_TAG_REPLY },
    { PSYC_C2STRI("_target_forward"),		PSYC_RVAR_TARGET_FORWARD },
    { PSYC_C2STRI("_understand_modules"),	PSYC_RVAR_UNDERSTAND_MODULES },
    { PSYC_C2STRI("_using_modules"),		PSYC_RVAR_USING_MODULES },
*/
};
const size_t psyc_rvars_num = PSYC_NUM_ELEM(psyc_rvars);

// Variable types in alphabetical order.
const PsycMapInt psyc_var_types[] = {
    { PSYC_C2STRI("_amount"),	PSYC_TYPE_AMOUNT },
    { PSYC_C2STRI("_color"),	PSYC_TYPE_COLOR },
    { PSYC_C2STRI("_date"),	PSYC_TYPE_DATE },
    { PSYC_C2STRI("_degree"),	PSYC_TYPE_DEGREE },
    { PSYC_C2STRI("_dict"),	PSYC_TYPE_DICT },
    { PSYC_C2STRI("_entity"),	PSYC_TYPE_ENTITY },
    { PSYC_C2STRI("_flag"),	PSYC_TYPE_FLAG },
    { PSYC_C2STRI("_language"),	PSYC_TYPE_LANGUAGE },
    { PSYC_C2STRI("_list"),	PSYC_TYPE_LIST },
    { PSYC_C2STRI("_nick"),	PSYC_TYPE_NICK },
    { PSYC_C2STRI("_page"),	PSYC_TYPE_PAGE },
    { PSYC_C2STRI("_struct"),	PSYC_TYPE_STRUCT },
    { PSYC_C2STRI("_time"),	PSYC_TYPE_TIME },
    { PSYC_C2STRI("_uniform"),	PSYC_TYPE_UNIFORM },
};
const size_t psyc_var_types_num = PSYC_NUM_ELEM(psyc_var_types);

/// Method names in alphabetical order.
const PsycMapInt psyc_methods[] = {
    { PSYC_C2STRI("_data"),			PSYC_MC_DATA },
    { PSYC_C2STRI("_echo_context_enter"),	PSYC_MC_ECHO_CONTEXT_ENTER },
    { PSYC_C2STRI("_echo_context_leave"),	PSYC_MC_ECHO_CONTEXT_LEAVE },
    { PSYC_C2STRI("_echo_hello"),		PSYC_MC_ECHO_HELLO },
    { PSYC_C2STRI("_echo"),			PSYC_MC_ECHO },
    { PSYC_C2STRI("_failure_alias_nonexistant"),PSYC_MC_FAILURE_ALIAS_NONEXISTANT },
    { PSYC_C2STRI("_failure_alias_unavailable"),PSYC_MC_FAILURE_ALIAS_UNAVAILABLE },
    { PSYC_C2STRI("_failure"),			PSYC_MC_FAILURE },
    { PSYC_C2STRI("_info"),			PSYC_MC_INFO },
    { PSYC_C2STRI("_message_action"),		PSYC_MC_MESSAGE_ACTION },
    { PSYC_C2STRI("_message"),			PSYC_MC_MESSAGE },
    { PSYC_C2STRI("_notice_alias_add"),		PSYC_MC_NOTICE_ALIAS_ADD },
    { PSYC_C2STRI("_notice_alias_change"),	PSYC_MC_NOTICE_ALIAS_CHANGE },
    { PSYC_C2STRI("_notice_alias_remove"),	PSYC_MC_NOTICE_ALIAS_REMOVE },
    { PSYC_C2STRI("_notice_context_enter"),	PSYC_MC_NOTICE_CONTEXT_ENTER },
    { PSYC_C2STRI("_notice_context_leave"),	PSYC_MC_NOTICE_CONTEXT_LEAVE },
    { PSYC_C2STRI("_notice_friendship"),	PSYC_MC_NOTICE_FRIENDSHIP },
    { PSYC_C2STRI("_notice_link"),		PSYC_MC_NOTICE_LINK },
    { PSYC_C2STRI("_notice_peer_connect"),	PSYC_MC_NOTICE_PEER_CONNECT },
    { PSYC_C2STRI("_notice_peer_disconnect"),	PSYC_MC_NOTICE_PEER_DISCONNECT },
    { PSYC_C2STRI("_notice_set"),		PSYC_MC_NOTICE_SET },
    { PSYC_C2STRI("_notice_unlink"),		PSYC_MC_NOTICE_UNLINK },
    { PSYC_C2STRI("_notice"),			PSYC_MC_NOTICE },
    { PSYC_C2STRI("_request_context_enter"),	PSYC_MC_REQUEST_CONTEXT_ENTER },
    { PSYC_C2STRI("_request_context_leave"),	PSYC_MC_REQUEST_CONTEXT_LEAVE },
    { PSYC_C2STRI("_request_frienship"),	PSYC_MC_REQUEST_FRIENDSHIP },
    { PSYC_C2STRI("_request"),			PSYC_MC_REQUEST },
    { PSYC_C2STRI("_status_contexts_entered"),	PSYC_MC_STATUS_CONTEXTS_ENTERED },
    { PSYC_C2STRI("_status_hello"),		PSYC_MC_STATUS_HELLO },
    { PSYC_C2STRI("_status"),			PSYC_MC_STATUS },
    { PSYC_C2STRI("_warning"),			PSYC_MC_WARNING },
};
const size_t psyc_methods_num = PSYC_NUM_ELEM(psyc_methods);

/**
 * Get the method, its family and its flags.
 */
PsycMethod
psyc_method (char *method, size_t methodlen, PsycMethod *family, unsigned int *flag)
{
    int mc = psyc_map_lookup_int(psyc_methods, psyc_methods_num,
				  method, methodlen, PSYC_YES);

    switch (mc) {
    case PSYC_MC_DATA:
	*family = PSYC_MC_DATA;
	*flag = 0;
	break;
    case PSYC_MC_ECHO:
    case PSYC_MC_ECHO_CONTEXT_ENTER:
    case PSYC_MC_ECHO_CONTEXT_LEAVE:
    case PSYC_MC_ECHO_HELLO:
	*family = PSYC_MC_ECHO;
	*flag = PSYC_METHOD_TEMPLATE | PSYC_METHOD_REPLY | PSYC_METHOD_VISIBLE;
	break;
    case PSYC_MC_ERROR:
	*family = PSYC_MC_ERROR;
	*flag = PSYC_METHOD_TEMPLATE | PSYC_METHOD_REPLY | PSYC_METHOD_VISIBLE
	    | PSYC_METHOD_LOGGABLE;
	break;
    case PSYC_MC_FAILURE:
    case PSYC_MC_FAILURE_ALIAS_NONEXISTANT:
    case PSYC_MC_FAILURE_ALIAS_UNAVAILABLE:
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
    case PSYC_MC_NOTICE:
    case PSYC_MC_NOTICE_ALIAS_ADD:
    case PSYC_MC_NOTICE_ALIAS_CHANGE:
    case PSYC_MC_NOTICE_ALIAS_REMOVE:
    case PSYC_MC_NOTICE_CONTEXT_ENTER:
    case PSYC_MC_NOTICE_CONTEXT_LEAVE:
    case PSYC_MC_NOTICE_FRIENDSHIP:
    case PSYC_MC_NOTICE_LINK:
    case PSYC_MC_NOTICE_SET:
    case PSYC_MC_NOTICE_UNLINK:
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
    case PSYC_MC_STATUS_HELLO:
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
