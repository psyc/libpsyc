use std::os::raw::c_int;

bitflags! {
    pub flags PsycMethodFlags: c_int {
        const PSYC_METHOD_TEMPLATE  = 1 << 0,
        const PSYC_METHOD_REPLY     = 1 << 1,
        const PSYC_METHOD_VISIBLE   = 1 << 2,
        const PSYC_METHOD_LOGGABLE  = 1 << 3,
        const PSYC_METHOD_MANUAL    = 1 << 4
    }
}

#[repr(C)]
#[derive(Debug, PartialEq)]
pub enum PsycMethod {
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
}

#[derive(Debug, PartialEq)]
pub struct MethodInfo {
    pub lookup_result: PsycMethod,
    pub family: PsycMethod,
    pub flags: PsycMethodFlags
}
