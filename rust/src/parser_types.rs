#![allow(non_camel_case_types)]
use types::*;

enum PsycPart { }
enum PsycListPart { }
enum PsycDictPart { }
enum PsycIndexPart { }
enum PsycUpdatePart { }

#[repr(C)]
pub struct PsycParseState {
    buffer: PsycString,
    cursor: usize,
    startc: usize,
    routinglen: usize,
    contentlen: usize,
    content_parsed: usize,
    valuelen: usize,
    value_parsed: usize,
    part: PsycPart,
    flags: u8,
    contentlen_found: u8,
    valuelen_found: u8
}

#[repr(C)]
pub struct PsycParseListState {
    buffer: PsycString,
    pub cursor: usize,
    startc: usize,
    list_type: PsycString,
    elemlen: usize,
    elem_parsed: usize,
    part: PsycListPart,
    elemlen_found: u8
}

#[repr(C)]
pub struct PsycParseDictState {
    buffer: PsycString,
    pub cursor: usize,
    startc: usize,
    elemlen: usize,
    elem_parsed: usize,
    part: PsycDictPart,
    elemlen_found: u8
}

#[repr(C)]
pub struct PsycParseIndexState { 
    buffer: PsycString,
    pub cursor: usize,
    startc: usize,
    elemlen: usize,
    elem_parsed: usize,
    part: PsycIndexPart,
    elemlen_found: u8
}

#[repr(C)]
pub struct PsycParseUpdateState {
    buffer: PsycString,
    pub cursor: usize,
    startc: usize,
    elemlen: usize,
    elem_parsed: usize,
    part: PsycUpdatePart,
    elemlen_found: u8
}

#[repr(C)]
pub enum PsycParseFlag {
    /// Default Flag. Parse everything.
    PSYC_PARSE_ALL = 0,
    /// Parse only the header
    PSYC_PARSE_ROUTING_ONLY = 1,
    /// Parse only the content.
    /// Parsing starts at the content and the content must be complete.
    PSYC_PARSE_START_AT_CONTENT = 2,
}

#[derive(Debug)]
#[repr(C)]
pub enum PsycParseRC {
    /// Error, no length is set for a modifier which is longer than PSYC_MODIFIER_SIZE_THRESHOLD.
    PSYC_PARSE_ERROR_MOD_NO_LEN = -10,
    /// Error, no length is set for the content but it is longer than PSYC_CONTENT_SIZE_THRESHOLD.
    PSYC_PARSE_ERROR_NO_LEN = -9,
    /// Error, packet is not ending with a valid delimiter.
    PSYC_PARSE_ERROR_END = -8,
    /// Error, expected NL after the method.
    PSYC_PARSE_ERROR_METHOD = -7,
    /// Error, expected NL after a modifier.
    PSYC_PARSE_ERROR_MOD_NL = -6,
    /// Error, modifier length is not numeric.
    PSYC_PARSE_ERROR_MOD_LEN = -5,
    /// Error, expected TAB before modifier value.
    PSYC_PARSE_ERROR_MOD_TAB = -4,
    /// Error, modifier name is missing.
    PSYC_PARSE_ERROR_MOD_NAME = -3,
    /// Error, expected NL after the content length.
    PSYC_PARSE_ERROR_LENGTH = -2,
    /// Error in packet.
    PSYC_PARSE_ERROR = -1,
    /// Buffer contains insufficient amount of data.
    /// Fill another buffer and concatenate it with the end of the current buffer,
    /// from the cursor position to the end.
    PSYC_PARSE_INSUFFICIENT = 1,
    /// Routing modifier parsing done.
    /// Operator, name & value contains the respective parts.
    PSYC_PARSE_ROUTING = 2,
    /// State sync operation.
    PSYC_PARSE_STATE_RESYNC = 3,
    /// State reset operation.
    PSYC_PARSE_STATE_RESET = 4,
    /// Start of an incomplete entity modifier.
    /// Operator & name are complete, value is incomplete.
    PSYC_PARSE_ENTITY_START = 5,
    /// Continuation of an incomplete entity modifier.
    PSYC_PARSE_ENTITY_CONT = 6,
    /// End of an incomplete entity modifier.
    PSYC_PARSE_ENTITY_END = 7,
    /// Entity modifier parsing done in one go.
    /// Operator, name & value contains the respective parts.
    PSYC_PARSE_ENTITY = 8,
    /// Start of an incomplete body.
    /// Name contains method, value contains part of the body.
    /// Used when packet length is given
    PSYC_PARSE_BODY_START = 9,
    /// Continuation of an incomplete body.
    /// Used when packet length is given
    PSYC_PARSE_BODY_CONT = 10,
    /// End of an incomplete body.
    /// Used when packet length is given
    PSYC_PARSE_BODY_END = 11,
    /// Body parsing done in one go, name contains method, value contains body.
    PSYC_PARSE_BODY = 12,
    ///// Start of an incomplete content, value contains part of content.
    ///// Used when PSYC_PARSE_ROUTING_ONLY is set.
    //PSYC_PARSE_CONTENT_START = 9,
    ///// Continuation of an incomplete content.
    ///// Used when PSYC_PARSE_ROUTING_ONLY is set.
    //PSYC_PARSE_CONTENT_CONT = 10,
    ///// End of an incomplete content.
    ///// Used when PSYC_PARSE_ROUTING_ONLY is set.
    //PSYC_PARSE_CONTENT_END = 11,
    ///// Content parsing done in one go, value contains the whole content.
    ///// Used when PSYC_PARSE_ROUTING_ONLY is set.
    //PSYC_PARSE_CONTENT = 12,
    /// Finished parsing packet.
    PSYC_PARSE_COMPLETE = 13,
}

#[repr(C)]
pub enum PsycParseListRC {
    /// Error, no length is set for an element which is longer than PSYC_ELEM_SIZE_THRESHOLD.
    PSYC_PARSE_LIST_ERROR_ELEM_NO_LEN = -6,
    PSYC_PARSE_LIST_ERROR_ELEM_LENGTH = -5,
    PSYC_PARSE_LIST_ERROR_ELEM_TYPE = -4,
    PSYC_PARSE_LIST_ERROR_ELEM_START = -3,
    PSYC_PARSE_LIST_ERROR_TYPE = -2,
    PSYC_PARSE_LIST_ERROR = -1,
    /// Reached end of buffer.
    /// Buffer contains insufficient amount of data.
    /// Fill another buffer and concatenate it with the end of the current buffer,
    /// from the cursor position to the end.
    PSYC_PARSE_LIST_INSUFFICIENT = 1,
    /// Completed parsing the default type of the list.
    PSYC_PARSE_LIST_TYPE = 2,
    /// Start of an element is parsed but still not complete.
    PSYC_PARSE_LIST_ELEM_START = 3,
    /// Continuation of an incomplete element.
    PSYC_PARSE_LIST_ELEM_CONT = 4,
    /// Element parsing completed.
    PSYC_PARSE_LIST_ELEM_END = 5,
    /// Completed parsing a list element.
    PSYC_PARSE_LIST_ELEM = 6,
    /// Completed parsing the last element in the list.
    PSYC_PARSE_LIST_ELEM_LAST = 7,
    /// Reached end of buffer.
    PSYC_PARSE_LIST_END = 8,
}

#[derive(Debug)]
#[repr(C)]
pub enum PsycParseDictRC {
    PSYC_PARSE_DICT_ERROR_VALUE = -9,
    PSYC_PARSE_DICT_ERROR_VALUE_LENGTH = -8,
    PSYC_PARSE_DICT_ERROR_VALUE_TYPE = -7,
    PSYC_PARSE_DICT_ERROR_VALUE_START = -6,
    PSYC_PARSE_DICT_ERROR_KEY = -5,
    PSYC_PARSE_DICT_ERROR_KEY_LENGTH = -4,
    PSYC_PARSE_DICT_ERROR_KEY_START = -3,
    PSYC_PARSE_DICT_ERROR_TYPE = -2,
    PSYC_PARSE_DICT_ERROR = -1,
    /// Reached end of buffer.
    /// Buffer contains insufficient amount of data.
    /// Fill another buffer and concatenate it with the end of the current buffer,
    /// from the cursor position to the end.
    PSYC_PARSE_DICT_INSUFFICIENT = 1,
    /// Completed parsing the default type of the dict.
    PSYC_PARSE_DICT_TYPE = 2,
    /// Start of a key is parsed but still not complete.
    PSYC_PARSE_DICT_KEY_START = 3,
    /// Continuation of an incomplete key.
    PSYC_PARSE_DICT_KEY_CONT = 4,
    /// Last continuation of a key.
    PSYC_PARSE_DICT_KEY_END = 5,
    /// Completed parsing a key in one go.
    PSYC_PARSE_DICT_KEY = 6,
    /// Start of a value is parsed but still not complete.
    PSYC_PARSE_DICT_VALUE_START = 7,
    /// Continuation of an incomplete value.
    PSYC_PARSE_DICT_VALUE_CONT = 8,
    /// Last continuation of a value.
    PSYC_PARSE_DICT_VALUE_END = 9,
    /// Completed parsing a value.
    PSYC_PARSE_DICT_VALUE = 10,
    /// Completed parsing the last value.
    PSYC_PARSE_DICT_VALUE_LAST = 11,
    /// Reached end of buffer.
    PSYC_PARSE_DICT_END = 12,
}

#[repr(C)]
pub enum PsycParseIndexRC {
    PSYC_PARSE_INDEX_ERROR_DICT = -6,
    PSYC_PARSE_INDEX_ERROR_DICT_LENGTH = -5,
    PSYC_PARSE_INDEX_ERROR_STRUCT = -4,
    PSYC_PARSE_INDEX_ERROR_LIST = -3,
    PSYC_PARSE_INDEX_ERROR_TYPE = -2,
    PSYC_PARSE_INDEX_ERROR = -1,
    /// Reached end of buffer.
    /// Buffer contains insufficient amount of data.
    /// Fill another buffer and concatenate it with the end of the current buffer,
    /// from the cursor position to the end.
    PSYC_PARSE_INDEX_INSUFFICIENT = 1,
    // Completed parsing a list index.
    PSYC_PARSE_INDEX_LIST = 3,
    // Completed parsing a list index at the end of the buffer.
    PSYC_PARSE_INDEX_LIST_LAST = 4,
    // Completed parsing a struct element name.
    PSYC_PARSE_INDEX_STRUCT = 5,
    // Completed parsing a struct element name at the end of the buffer.
    PSYC_PARSE_INDEX_STRUCT_LAST = 6,
    /// Start of a dict key is parsed but still not complete.
    PSYC_PARSE_INDEX_DICT_START = 7,
    /// Continuation of an incomplete dict key.
    PSYC_PARSE_INDEX_DICT_CONT = 8,
    /// Last continuation of a dict key.
    PSYC_PARSE_INDEX_DICT_END = 9,
    /// Completed parsing a dict key in one go.
    PSYC_PARSE_INDEX_DICT = 10,
    /// Reached end of buffer.
    PSYC_PARSE_INDEX_END = 11,
}

#[repr(C)]
pub enum PsycParseUpdateRC {
    PSYC_PARSE_UPDATE_ERROR_VALUE = -24,
    PSYC_PARSE_UPDATE_ERROR_LENGTH = -23,
    PSYC_PARSE_UPDATE_ERROR_TYPE = -22,
    PSYC_PARSE_UPDATE_ERROR_OPER = -21,
    PSYC_PARSE_UPDATE_ERROR = -1,
    /// Reached end of buffer.
    /// Buffer contains insufficient amount of data.
    /// Fill another buffer and concatenate it with the end of the current buffer,
    /// from the cursor position to the end.
    PSYC_PARSE_UPDATE_INSUFFICIENT = 1,

    // Completed parsing a list index.
    PSYC_PARSE_UPDATE_INDEX_LIST = 3,
    // Completed parsing a struct element name.
    PSYC_PARSE_UPDATE_INDEX_STRUCT = 5,
    /// Start of a dict key is parsed but still not complete.
    PSYC_PARSE_UPDATE_INDEX_DICT_START = 7,
    /// Continuation of an incomplete dict key.
    PSYC_PARSE_UPDATE_INDEX_DICT_CONT = 8,
    /// Last continuation of a dict key.
    PSYC_PARSE_UPDATE_INDEX_DICT_END = 9,
    /// Completed parsing a dict key in one go.
    PSYC_PARSE_UPDATE_INDEX_DICT = 10,

    /// Completed parsing the type.
    PSYC_PARSE_UPDATE_TYPE = 21,
    /// Completed parsing the type and reached end of buffer.
    PSYC_PARSE_UPDATE_TYPE_END = 22,
    /// Start of the value is parsed but still not complete.
    PSYC_PARSE_UPDATE_VALUE_START = 23,
    /// Continuation of incomplete value.
    PSYC_PARSE_UPDATE_VALUE_CONT = 24,
    /// Last continuation of the value.
    PSYC_PARSE_UPDATE_VALUE_END = 25,
    /// Completed parsing the value in one go.
    PSYC_PARSE_UPDATE_VALUE = 26,
    /// Reached end of buffer.
    PSYC_PARSE_UPDATE_END = 27,
}




