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

#ifndef PSYC_METHOD_H
#define PSYC_METHOD_H

/** Characteristics attributed to certain method families. */
typedef enum {
    PSYC_METHOD_TEMPLATE = 1 << 0,
    PSYC_METHOD_REPLY    = 1 << 1,
    PSYC_METHOD_VISIBLE  = 1 << 2,
    PSYC_METHOD_LOGGABLE = 1 << 3,
    PSYC_METHOD_MANUAL   = 1 << 4,
} PsycMethodFlag;

/**
 * This is a preliminary set of methods, you will encounter a lot more in
 * the wild. The definitive standardization and definition of compact methods
 * will follow. "mc" is an old abbreviation for method.
 */
typedef enum {
    PSYC_MC_UNKNOWN,

    PSYC_MC_CONVERSE,
    PSYC_MC_DATA,
    PSYC_MC_ECHO,
    PSYC_MC_ECHO_CONTEXT_ENTER,
    PSYC_MC_ECHO_CONTEXT_LEAVE,
    PSYC_MC_ECHO_HELLO,
    PSYC_MC_ERROR,
    PSYC_MC_FAILURE,
    PSYC_MC_FAILURE_ALIAS_NONEXISTANT,
    PSYC_MC_FAILURE_ALIAS_UNAVAILABLE,
    PSYC_MC_INFO,
    PSYC_MC_MESSAGE,		// deprecated, use _converse
    PSYC_MC_MESSAGE_ACTION,	// deprecated, use _converse
    PSYC_MC_NOTICE,
    PSYC_MC_NOTICE_ALIAS_ADD,
    PSYC_MC_NOTICE_ALIAS_CHANGE,
    PSYC_MC_NOTICE_ALIAS_REMOVE,
    PSYC_MC_NOTICE_CONTEXT_ENTER,
    PSYC_MC_NOTICE_CONTEXT_LEAVE,
    PSYC_MC_NOTICE_FRIENDSHIP,
    PSYC_MC_NOTICE_LINK,
    PSYC_MC_NOTICE_PEER_CONNECT,
    PSYC_MC_NOTICE_PEER_DISCONNECT,
    PSYC_MC_NOTICE_SET,
    PSYC_MC_NOTICE_UNLINK,
    PSYC_MC_REQUEST,
    PSYC_MC_REQUEST_CONTEXT_ENTER,
    PSYC_MC_REQUEST_CONTEXT_LEAVE,
    PSYC_MC_REQUEST_FRIENDSHIP,
    PSYC_MC_STATUS,
    PSYC_MC_STATUS_CONTEXTS_ENTERED,
    PSYC_MC_STATUS_HELLO,
    PSYC_MC_WARNING,

    PSYC_METHODS_NUM,
} PsycMethod;

/* Same list of typical but non-final methods again */
typedef union PsycTemplates {
    PsycString a[PSYC_METHODS_NUM];
    struct {
	PsycString _;
	PsycString _converse;
	PsycString _data;
	PsycString _echo;
	PsycString _echo_context_enter;
	PsycString _echo_context_leave;
	PsycString _echo_hello;
	PsycString _error;
	PsycString _failure;
	PsycString _failure_alias_nonexistant;
	PsycString _failure_alias_unavailable;
	PsycString _info;
	PsycString _message;		// deprecated, use _converse
	PsycString _message_action;	// deprecated, use _converse
	PsycString _notice;
	PsycString _notice_alias_add;
	PsycString _notice_alias_change;
	PsycString _notice_alias_remove;
	PsycString _notice_context_enter;
	PsycString _notice_context_leave;
	PsycString _notice_friendship;
	PsycString _notice_link;
	PsycString _notice_peer_connect;
	PsycString _notice_peer_disconnect;
	PsycString _notice_set;
	PsycString _notice_unlink;
	PsycString _request;
	PsycString _request_context_enter;
	PsycString _request_context_leave;
	PsycString _request_friendship;
	PsycString _status;
	PsycString _status_contexts_entered;
	PsycString _status_hello;
	PsycString _warning;
    } s;
} PsycTemplates;

#endif
