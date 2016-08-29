use types::*;
use parser_types::*;
use std::mem;
use std::slice;
use std::os::raw::c_char;

extern "C" {
    fn psyc_parse_state_init(state: *mut PsycParseState, flags: u8);
    fn psyc_parse_buffer_set(state: *mut PsycParseState, buffer: *const c_char, length: usize);
    fn psyc_parse_list_state_init(state: *mut PsycParseListState);
    fn psyc_parse_list_buffer_set(state: *mut PsycParseListState, buffer: *const c_char, length: usize);
    fn psyc_parse_dict_state_init(state: *mut PsycParseDictState);
    fn psyc_parse_dict_buffer_set(state: *mut PsycParseDictState, buffer: *const c_char, length: usize);
    fn psyc_parse_index_state_init(state: *mut PsycParseIndexState);
    fn psyc_parse_index_buffer_set(state: *mut PsycParseIndexState, buffer: *const c_char, length: usize);
    fn psyc_parse_update_state_init(state: *mut PsycParseUpdateState);
    fn psyc_parse_update_buffer_set(state: *mut PsycParseUpdateState, buffer: *const c_char, length: usize);
    fn psyc_parse_content_length(state: *mut PsycParseState) -> usize;
    fn psyc_parse_content_length_found(state: *mut PsycParseState) -> bool;
    fn psyc_parse_value_length(state: *mut PsycParseState) -> usize;
    fn psyc_parse_value_length_found(state: *mut PsycParseState) -> bool;
    fn psyc_parse_cursor(state: *mut PsycParseState) -> usize;
    fn psyc_parse_buffer_length(state: *mut PsycParseState) -> usize;
    fn psyc_parse_remaining_length(state: *mut PsycParseState) -> usize;
    fn psyc_parse_remaining_buffer(state: *mut PsycParseState) -> *const c_char;
    fn psyc_parse(state: *mut PsycParseState,
                  oper: *mut c_char,
                  name: *mut PsycString,
                  value: *mut PsycString)
                  -> PsycParseRC;

    fn psyc_parse_list(state: *mut PsycParseListState,
                       list_type: *mut PsycString,
                       elem: *mut PsycString)
                       -> PsycParseListRC;

    fn psyc_parse_dict(state: *mut PsycParseDictState,
                       dict_type: *mut PsycString,
                       elem: *mut PsycString)
                       -> PsycParseDictRC;

    fn psyc_parse_index(state: *mut PsycParseIndexState,
                        idx: *mut PsycString)
                        -> PsycParseIndexRC;

    fn psyc_parse_update(state: *mut PsycParseUpdateState,
                         oper: *mut c_char,
                         value: *mut PsycString)
                         -> PsycParseUpdateRC;

    fn psyc_parse_uint(value: *const c_char, len: usize, n: *mut u64) -> usize;
    fn psyc_parse_list_index(value: *const c_char, len: usize, n: *mut i64) -> usize;
    fn psyc_is_oper(g: c_char) -> bool;
    fn psyc_is_numeric(c: c_char) -> bool;
    fn psyc_is_alpha(c: c_char) -> bool;
    fn psyc_is_alpha_numeric(c: c_char) -> bool;
    fn psyc_is_kw_char(c: c_char) -> bool;
    fn psyc_is_name_char(c: c_char) -> bool;
    fn psyc_is_host_char(c: c_char) -> bool;
    fn psyc_parse_keyword(data: *const c_char, len: usize) -> usize;
}

pub struct PsycParser<'a> {
    state: PsycParseState,
    operator: char,
    name: Option<&'a [u8]>,
    buffer: Option<&'a [u8]>,
    cursor: usize
}

//pub struct PsycListParser<'a> {
//    state: PsycParseListState,
//    parsed_list: Vec<Vec<u8>>,
//    buffer: &'a [u8]
//}
//
//pub struct PsycDictParser<'a> {
//    state: PsycParseDictState,
//    parsed_dict: Vec<(Vec<u8>, Vec<u8>)>,
//    buffer: &'a [u8]
//}
//
//// TODO: What data structures does the index parser need?
//pub struct PsycIndexParser {
//    state: PsycParseIndexState
//}
//
//// TODO: what data structures does the update parser need?
//pub struct PsycUpdateParser {
//    state: PsycParseUpdateState
//}

#[derive(Debug, PartialEq)]
pub enum PsycParserResult<'a> {
    StateSync,
    StateReset,
    Complete,
    InsufficientData,
    RoutingModifier {
        operator: char,
        name: &'a [u8],
        value: &'a [u8]
    },
    EntityModifier {
        operator: char,
        name: &'a [u8],
        value: &'a [u8]
    },
    Body {
        name: &'a [u8],
        value: &'a [u8]
    },
}

//#[derive(Debug, PartialEq)]
//pub enum PsycDictParserResult {
//    InsufficientData,
//    Dict {
//        data: Vec<(&'a [u8], &'a[u8])>
//    }
//}

#[repr(C)]
#[derive(Debug, PartialEq)]
pub enum PsycParserError {
    NoModifierLength = PsycParseRC::PSYC_PARSE_ERROR_MOD_NO_LEN as _,
    NoContentLength = PsycParseRC::PSYC_PARSE_ERROR_NO_LEN as _,
    NoEndDelimiter = PsycParseRC::PSYC_PARSE_ERROR_END as _,
    NoNewlineAfterMethod = PsycParseRC::PSYC_PARSE_ERROR_METHOD as _,
    NoNewlineAfterModifier = PsycParseRC::PSYC_PARSE_ERROR_MOD_NL as _,
    InvalidModifierLength = PsycParseRC::PSYC_PARSE_ERROR_MOD_LEN as _,
    NoTabBeforeModifierValue = PsycParseRC::PSYC_PARSE_ERROR_MOD_TAB as _,
    NoModifierName = PsycParseRC::PSYC_PARSE_ERROR_MOD_NAME as _,
    NoNewlineAfterContentLength = PsycParseRC::PSYC_PARSE_ERROR_LENGTH as _,
    GenericError = PsycParseRC::PSYC_PARSE_ERROR as _,
}

impl<'a> PsycParser<'a> {
    /// Create a PsycParser
    pub fn new() -> Self {
        let mut state: PsycParseState;
        unsafe {
            state = mem::uninitialized();
            let state_ptr = &mut state as *mut PsycParseState;
            psyc_parse_state_init(state_ptr, PsycParseFlag::PSYC_PARSE_ALL as u8)
        }
        PsycParser{
            state: state,
            operator: '\0',
            name: None,
            buffer: None,
            cursor: 0
        }
    }

    /// Set a buffer of raw bytes for parsing
    pub fn set_buffer(&mut self, buffer: &'a [u8]) {
        self.buffer = Some(buffer);
        let state_ptr = &mut self.state as *mut PsycParseState;
        let buffer_ptr = &buffer[self.cursor] as *const u8 as *const c_char;
        unsafe {
            psyc_parse_buffer_set(state_ptr, buffer_ptr, buffer.len() - self.cursor)
        }
    }

    /// Parse the buffer previously set by set_buffer. Call repeatedly until the
    /// result is PsycParserResult::Complete or a PsycParserError.
    pub fn parse(&mut self)
                 -> Result<PsycParserResult<'a>, PsycParserError> {
        let state_ptr = &mut self.state as *mut PsycParseState;
        let mut name: PsycString;
        let mut value: PsycString;
        unsafe {
            name = mem::uninitialized();
            value = mem::uninitialized();
            let operator_ptr = &mut self.operator as *mut char as *mut c_char;
            let name_ptr = &mut name as *mut PsycString;
            let value_ptr = &mut value as *mut PsycString;
            let parse_result = psyc_parse(state_ptr, operator_ptr, name_ptr, value_ptr);
            self.cursor = self.cursor + psyc_parse_cursor(state_ptr);
            match parse_result {
                PsycParseRC::PSYC_PARSE_STATE_RESYNC =>
                    Ok(PsycParserResult::StateSync),

                PsycParseRC::PSYC_PARSE_STATE_RESET =>
                    Ok(PsycParserResult::StateReset),

                PsycParseRC::PSYC_PARSE_COMPLETE =>
                    Ok(PsycParserResult::Complete),

                PsycParseRC::PSYC_PARSE_ROUTING => {
                    let result = PsycParserResult::RoutingModifier {
                        operator: self.operator,
                        name: Self::cstring_to_slice(name.data, name.length),
                        value: Self::cstring_to_slice(value.data, value.length)
                    };
                    Ok(result)
                },

                PsycParseRC::PSYC_PARSE_ENTITY => {
                    let result = PsycParserResult::EntityModifier {
                        operator: self.operator,
                        name: Self::cstring_to_slice(name.data, name.length),
                        value: Self::cstring_to_slice(value.data, value.length)
                    };
                    Ok(result)
                },

                PsycParseRC::PSYC_PARSE_BODY => {
                    let result = PsycParserResult::Body {
                        name: Self::cstring_to_slice(name.data, name.length),
                        value: Self::cstring_to_slice(value.data, value.length)
                    };
                    Ok(result)
                },

                PsycParseRC::PSYC_PARSE_ENTITY_START |
                PsycParseRC::PSYC_PARSE_BODY_START => {
                    self.name = Some(Self::cstring_to_slice(name.data, name.length));
                    Ok(PsycParserResult::InsufficientData)
                },

                PsycParseRC::PSYC_PARSE_ENTITY_END => {
                    let result = PsycParserResult::EntityModifier {
                        operator: self.operator,
                        name: self.name.unwrap(),
                        value: Self::cstring_to_slice(value.data, value.length)
                    };
                    Ok(result)
                },

                PsycParseRC::PSYC_PARSE_BODY_END => {
                    let result = PsycParserResult::Body {
                        name: self.name.unwrap(),
                        value: Self::cstring_to_slice(value.data, value.length)
                    };
                    Ok(result)
                },

                PsycParseRC::PSYC_PARSE_INSUFFICIENT |
                PsycParseRC::PSYC_PARSE_ENTITY_CONT |
                PsycParseRC::PSYC_PARSE_BODY_CONT => {
                    Ok(PsycParserResult::InsufficientData)
                },

                _error => Err(mem::transmute(_error)),
            }
        }
    }

    unsafe fn cstring_to_slice(cstring: *const c_char, length: usize) -> &'a [u8] {
        slice::from_raw_parts(cstring as *const u8, length)
    }
}
