#ifndef PSYC_METHOD_H
#define PSYC_METHOD_H

typedef enum PsycMethodFlag {
    PSYC_METHOD_TEMPLATE = 1 << 0,
    PSYC_METHOD_REPLY    = 1 << 1,
    PSYC_METHOD_VISIBLE  = 1 << 2,
    PSYC_METHOD_LOGGABLE = 1 << 3,
    PSYC_METHOD_MANUAL   = 1 << 4,
} PsycMethodFlag;

typedef enum PsycMethod {
    PSYC_MC_UNKNOWN,

    PSYC_MC_DATA,
    PSYC_MC_ECHO,
    PSYC_MC_ECHO_CONTEXT_ENTER,
    PSYC_MC_ECHO_CONTEXT_LEAVE,
    PSYC_MC_ECHO_HELLO,
    PSYC_MC_ERROR,
    PSYC_MC_FAILURE,
    PSYC_MC_INFO,
    PSYC_MC_MESSAGE,
    PSYC_MC_MESSAGE_ACTION,
    PSYC_MC_MESSAGE_ECHO,
    PSYC_MC_MESSAGE_ECHO_ACTION,
    PSYC_MC_NOTICE,
    PSYC_MC_NOTICE_CONTEXT_ENTER,
    PSYC_MC_NOTICE_CONTEXT_LEAVE,
    PSYC_MC_NOTICE_LINK,
    PSYC_MC_REQUEST,
    PSYC_MC_REQUEST_CONTEXT_ENTER,
    PSYC_MC_REQUEST_CONTEXT_LEAVE,
    PSYC_MC_REQUEST_LINK,
    PSYC_MC_STATUS,
    PSYC_MC_STATUS_CONTEXTS_ENTERED,
    PSYC_MC_STATUS_HELLO,
    PSYC_MC_WARNING,

    PSYC_METHODS_NUM,
} PsycMethod;

typedef union PsycTemplates {
    PsycString a[PSYC_METHODS_NUM];
    struct {
	PsycString _;
	PsycString _data;
	PsycString _echo;
	PsycString _echo_context_enter;
	PsycString _echo_context_leave;
	PsycString _echo_hello;
	PsycString _error;
	PsycString _failure;
	PsycString _info;
	PsycString _message;
	PsycString _message_action;
	PsycString _message_echo;
	PsycString _message_echo_action;
	PsycString _notice;
	PsycString _notice_context_enter;
	PsycString _notice_context_leave;
	PsycString _notice_link;
	PsycString _request;
	PsycString _request_context_enter;
	PsycString _request_context_leave;
	PsycString _request_link;
	PsycString _status;
	PsycString _status_contexts_entered;
	PsycString _status_hello;
	PsycString _warning;
    } s;
} PsycTemplates;

#endif
