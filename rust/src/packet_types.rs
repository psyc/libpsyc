use std::os::raw::c_char;
use types::*;

#[repr(C)]
pub enum PsycModifierFlag {
    /// Modifier needs to be checked if it needs length.
    PSYC_MODIFIER_CHECK_LENGTH = 0,
    /// Modifier needs length.
    PSYC_MODIFIER_NEED_LENGTH = 1,
    /// Modifier doesn't need length.
    PSYC_MODIFIER_NO_LENGTH = 2,
    /// Routing modifier, which implies that it doesn't need length.
    PSYC_MODIFIER_ROUTING = 4,
}

#[repr(C)]
pub enum PsycElemFlag {
    /// Element needs to be checked if it needs length.
    PSYC_ELEM_CHECK_LENGTH = 0,
    /// Element needs length.
    PSYC_ELEM_NEED_LENGTH = 1,
    /// Element doesn't need length.
    PSYC_ELEM_NO_LENGTH = 2,
}

#[repr(C)]
pub enum PsycPacketFlag {
    /// Packet needs to be checked if it needs content length.
    PSYC_PACKET_CHECK_LENGTH = 0,
    /// Packet needs content length.
    PSYC_PACKET_NEED_LENGTH = 1,
    /// Packet doesn't need content length.
    PSYC_PACKET_NO_LENGTH = 2,
}

#[derive(Copy, Clone)]
#[repr(C)]
pub enum PsycOperator {
    PSYC_OPERATOR_SET = ':' as _,
    PSYC_OPERATOR_ASSIGN = '=' as _,
    PSYC_OPERATOR_AUGMENT = '+' as _,
    PSYC_OPERATOR_DIMINISH = '-' as _,
    PSYC_OPERATOR_UPDATE = '@' as _,
    PSYC_OPERATOR_QUERY = '?' as _,
}

#[repr(C)]
pub enum PsycStateOp {
    PSYC_STATE_NOOP = 0,
    PSYC_STATE_RESET = '=' as _,
    PSYC_STATE_RESYNC = '?' as _,
}

#[repr(C)]
pub enum PsycPacketId {
    PSYC_PACKET_ID_CONTEXT = 0,
    PSYC_PACKET_ID_SOURCE = 1,
    PSYC_PACKET_ID_TARGET = 2,
    PSYC_PACKET_ID_COUNTER = 3,
    PSYC_PACKET_ID_FRAGMENT = 4,
    PSYC_PACKET_ID_ELEMS = 5,
}

#[repr(C)]
pub struct PsycElem {
    pub elem_type: PsycString,
    pub value: PsycString,
    pub length: usize,
    pub flag: PsycElemFlag
}

#[repr(C)]
pub struct PsycDictKey {
    pub value: PsycString,
    pub length: usize,
    pub flag: PsycElemFlag
}

#[repr(C)]
pub struct PsycDictElem {
    pub value: PsycElem,
    pub key: PsycDictKey
}

#[repr(C)]
pub struct RawPsycDict {
    dict_type: PsycString,
    elems: *const PsycDictElem,
    num_elems: usize,
    pub length: usize
}

#[repr(C)]
pub struct RawPsycList {
    list_type: PsycString,
    elems: *const PsycElem,
    num_elems: usize,
    pub length: usize
}

#[repr(C)]
pub struct RawPsycModifier {
    name: PsycString,
    value: PsycString,
    flag: PsycModifierFlag,
    oper: c_char
}

#[repr(C)]
pub struct PsycHeader {
    lines: usize,
    modifiers: *mut RawPsycModifier
}

#[repr(C)]
pub struct RawPsycPacket {
    routing: PsycHeader,
    entity: PsycHeader,
    method: PsycString,
    data: PsycString,
    content: PsycString,
    routinglen: usize,
    contentlen: usize,
    pub length: usize,
    stateop: PsycStateOp,
    flag: PsycPacketFlag
}

#[repr(C)]
pub enum PsycRenderRC {
    /// Error, method is missing, but data is present.
    PSYC_RENDER_ERROR_METHOD_MISSING = -3,
    /// Error, a modifier name is missing.
    PSYC_RENDER_ERROR_MODIFIER_NAME_MISSING = -2,
    /// Error, buffer is too small to render the packet.
    PSYC_RENDER_ERROR = -1,
    /// Packet is rendered successfully in the buffer.
    PSYC_RENDER_SUCCESS = 0,
}



