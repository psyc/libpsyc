use types::*;
use parser_types::*;
use util;
use std::mem;
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
    fn psyc_parse_cursor(state: *const PsycParseState) -> usize;
    fn psyc_parse_buffer_length(state: *const PsycParseState) -> usize;
    fn psyc_parse_remaining_length(state: *const PsycParseState) -> usize;
    fn psyc_parse_remaining_buffer(state: *const PsycParseState) -> *const c_char;
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

pub struct PsycParser {
    state: PsycParseState
}

pub struct PsycListParser {
    state: PsycParseListState
}

pub struct PsycDictParser<'a> {
    state: PsycParseDictState,
    parsed_key: Option<&'a [u8]>,
    buffer: Option<&'a [u8]>,
    cursor: usize
}
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
    EntityModifierStart {
        operator: char,
        name: &'a [u8],
        value_part: &'a [u8]
    },
    EntityModifierCont {
        value_part: &'a [u8]
    },
    EntityModifierEnd {
        value_part: &'a [u8]
    },
    Body {
        name: &'a [u8],
        value: &'a [u8]
    },
    BodyStart {
        name: &'a [u8],
        value_part: &'a [u8]
    },
    BodyCont {
        value_part: &'a [u8]
    },
    BodyEnd {
        value_part: &'a [u8]
    }
}

#[derive(Debug, PartialEq)]
pub enum PsycListParserResult<'a> {
    Complete,
    InsufficientData,
    ListElement {
        value: &'a [u8]
    },
    ListElementStart {
        value_part: &'a [u8]
    },
    ListElementCont {
        value_part: &'a [u8]
    },
    ListElementEnd {
        value_part: &'a [u8]
    }
}

#[derive(Debug, PartialEq)]
pub enum PsycDictParserResult<'a> {
    Complete,
    InsufficientData,
    DictEntry {
        key: &'a [u8],
        value: &'a [u8]
    },
    DictEntryStart {
        key: &'a [u8],
        value_part: &'a [u8]
    },
    DictEntryCont {
        value_part: &'a [u8]
    },
    DictEntryEnd {
        value_part: &'a [u8]
    }
}

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

#[repr(C)]
#[derive(Debug, PartialEq)]
pub enum PsycListParserError {
    NoElementLength = PsycParseListRC::PSYC_PARSE_LIST_ERROR_ELEM_NO_LEN as _,
    InvalidElementLength = PsycParseListRC::PSYC_PARSE_LIST_ERROR_ELEM_LENGTH as _,
    InvalidElementType = PsycParseListRC::PSYC_PARSE_LIST_ERROR_ELEM_TYPE as _,
    InvalidElementStart = PsycParseListRC::PSYC_PARSE_LIST_ERROR_ELEM_START as _,
    InvalidType = PsycParseListRC::PSYC_PARSE_LIST_ERROR_TYPE as _,
    GenericError = PsycParseListRC::PSYC_PARSE_LIST_ERROR as _,
}

#[repr(C)]
#[derive(Debug, PartialEq)]
pub enum PsycDictParserError {
    InvalidValue = PsycParseDictRC::PSYC_PARSE_DICT_ERROR_VALUE as _,
    InvalidValueLength = PsycParseDictRC::PSYC_PARSE_DICT_ERROR_VALUE_LENGTH as _,
    InvalidValueType = PsycParseDictRC::PSYC_PARSE_DICT_ERROR_VALUE_TYPE as _,
    InvalidValueStart = PsycParseDictRC::PSYC_PARSE_DICT_ERROR_VALUE_START as _,
    InvalidKey = PsycParseDictRC::PSYC_PARSE_DICT_ERROR_KEY as _,
    InvalidKeyLength = PsycParseDictRC::PSYC_PARSE_DICT_ERROR_KEY_LENGTH as _,
    InvalidKeyStart = PsycParseDictRC::PSYC_PARSE_DICT_ERROR_KEY_START as _,
    InvalidKeyType = PsycParseDictRC::PSYC_PARSE_DICT_ERROR_TYPE as _,
    GenericError = PsycParseDictRC::PSYC_PARSE_DICT_ERROR as _,
    NoBuffer
}

impl<'a> PsycParser {
    /// Create a PsycParser
    pub fn new() -> Self {
        let mut state: PsycParseState;
        unsafe {
            state = mem::uninitialized();
            let state_ptr = &mut state as *mut PsycParseState;
            psyc_parse_state_init(state_ptr, PsycParseFlag::PSYC_PARSE_ALL as u8)
        }
        PsycParser {
            state: state
        }
    }

    /// Parse the buffer previously set by set_buffer. Call repeatedly until the
    /// result is PsycParserResult::Complete or a PsycParserError.
    pub fn parse(&mut self) -> Result<PsycParserResult<'a>, PsycParserError> {
        let state_ptr = &mut self.state as *mut PsycParseState;
        let mut operator = '\0';
        let mut name: PsycString;
        let mut value: PsycString;
        unsafe {
            name = mem::uninitialized();
            value = mem::uninitialized();
            let operator_ptr = &mut operator as *mut char as *mut c_char;
            let name_ptr = &mut name as *mut PsycString;
            let value_ptr = &mut value as *mut PsycString;
            let parse_result = psyc_parse(state_ptr, operator_ptr, name_ptr, value_ptr);
            match parse_result {
                PsycParseRC::PSYC_PARSE_STATE_RESYNC =>
                    Ok(PsycParserResult::StateSync),

                PsycParseRC::PSYC_PARSE_STATE_RESET =>
                    Ok(PsycParserResult::StateReset),

                PsycParseRC::PSYC_PARSE_COMPLETE =>
                    Ok(PsycParserResult::Complete),

                PsycParseRC::PSYC_PARSE_INSUFFICIENT =>
                    Ok(PsycParserResult::InsufficientData),

                PsycParseRC::PSYC_PARSE_ROUTING => {
                    let result = PsycParserResult::RoutingModifier {
                        operator: operator,
                        name: util::cstring_to_slice(name.data, name.length),
                        value: util::cstring_to_slice(value.data, value.length)
                    };
                    Ok(result)
                },

                PsycParseRC::PSYC_PARSE_ENTITY => {
                    let result = PsycParserResult::EntityModifier {
                        operator: operator,
                        name: util::cstring_to_slice(name.data, name.length),
                        value: util::cstring_to_slice(value.data, value.length)
                    };
                    Ok(result)
                },

                PsycParseRC::PSYC_PARSE_ENTITY_START => {
                    let result = PsycParserResult::EntityModifierStart {
                        operator: operator,
                        name: util::cstring_to_slice(name.data, name.length),
                        value_part: util::cstring_to_slice(value.data, value.length)
                    };
                    Ok(result)
                },

                PsycParseRC::PSYC_PARSE_ENTITY_CONT => {
                    let result = PsycParserResult::EntityModifierCont {
                        value_part: util::cstring_to_slice(value.data, value.length)
                    };
                    Ok(result)
                },

                PsycParseRC::PSYC_PARSE_ENTITY_END => {
                    let result = PsycParserResult::EntityModifierEnd {
                        value_part: util::cstring_to_slice(value.data, value.length)
                    };
                    Ok(result)
                }

                PsycParseRC::PSYC_PARSE_BODY => {
                    let result = PsycParserResult::Body {
                        name: util::cstring_to_slice(name.data, name.length),
                        value: util::cstring_to_slice(value.data, value.length)
                    };
                    Ok(result)
                },

                PsycParseRC::PSYC_PARSE_BODY_START => {
                    let result = PsycParserResult::BodyStart {
                        name: util::cstring_to_slice(name.data, name.length),
                        value_part: util::cstring_to_slice(value.data, value.length)
                    };
                    Ok(result)
                },
                PsycParseRC::PSYC_PARSE_BODY_CONT => {
                    let result = PsycParserResult::BodyCont {
                        value_part: util::cstring_to_slice(value.data, value.length)
                    };
                    Ok(result)
                },

                PsycParseRC::PSYC_PARSE_BODY_END => {
                    let result = PsycParserResult::BodyEnd {
                        value_part: util::cstring_to_slice(value.data, value.length)
                    };
                    Ok(result)
                }

                _error => Err(mem::transmute(_error)),
            }
        }
    }
}

impl<'a> Parser<'a> for PsycParser {
    fn set_buffer(&mut self, buffer: &'a [u8]) {
        let state_ptr = &mut self.state as *mut PsycParseState;
        let buffer_ptr = buffer.as_ptr() as *const c_char;
        unsafe {
            psyc_parse_buffer_set(state_ptr, buffer_ptr, buffer.len())
        }
    }

    fn unparsed_position(&self) -> usize {
        unsafe {
            psyc_parse_cursor(&self.state as *const PsycParseState)
        }
    }

    fn unparsed_length(&self) -> usize {
        unsafe {
            psyc_parse_remaining_length(&self.state as *const PsycParseState)
        }
    }
}

impl PsycListParser {
    pub fn new() -> Self {
        let mut state: PsycParseListState;
        unsafe {
            state = mem::uninitialized();
            let state_ptr = &mut state as *mut PsycParseListState;
            psyc_parse_list_state_init(state_ptr)
        }
        PsycListParser {
            state: state
        }
    }

    pub fn parse<'a>(&mut self) -> Result<PsycListParserResult<'a>, PsycListParserError> {
        let state_ptr = &mut self.state as *mut PsycParseListState;
        let mut list_type: PsycString;
        let mut element: PsycString;
        unsafe {
            list_type = mem::uninitialized();
            element = mem::uninitialized();
            let list_type_ptr = &mut list_type as *mut PsycString;
            let element_ptr = &mut element as *mut PsycString;
            loop {
                let parse_result = psyc_parse_list(state_ptr, list_type_ptr, element_ptr);
                println!("parse_result: {:?}", parse_result);
                println!("cursor: {}", self.state.cursor);
                match parse_result {
                    PsycParseListRC::PSYC_PARSE_LIST_END =>
                        return Ok(PsycListParserResult::Complete),
                    
                    PsycParseListRC::PSYC_PARSE_LIST_INSUFFICIENT =>
                        return Ok(PsycListParserResult::InsufficientData),

                    PsycParseListRC::PSYC_PARSE_LIST_ELEM_LAST |
                    PsycParseListRC::PSYC_PARSE_LIST_ELEM => {
                        let result = PsycListParserResult::ListElement {
                            value: util::cstring_to_slice(element.data, element.length)
                        };
                        return Ok(result)
                    },

                    PsycParseListRC::PSYC_PARSE_LIST_ELEM_START => {
                        let result = PsycListParserResult::ListElementStart {
                            value_part: util::cstring_to_slice(element.data, element.length)
                        };
                        return Ok(result)
                    },

                    PsycParseListRC::PSYC_PARSE_LIST_ELEM_CONT => {
                        let result = PsycListParserResult::ListElementCont {
                            value_part: util::cstring_to_slice(element.data, element.length)
                        };
                        return Ok(result)
                    },

                    PsycParseListRC::PSYC_PARSE_LIST_ELEM_END => {
                        let result = PsycListParserResult::ListElementEnd {
                            value_part: util::cstring_to_slice(element.data, element.length)
                        };
                        return Ok(result)
                    },

                    //PsycParseListRC::PSYC_PARSE_LIST_ELEM_LAST => {
                    //    let result: PsycListParserResult;
                    //    if ! self.finished {
                    //        result = PsycListParserResult::ListElement {
                    //            value: util::cstring_to_slice(element.data, element.length)
                    //        };
                    //        self.finished = true
                    //    } else {
                    //        result = PsycListParserResult::Complete
                    //    }
                    //    return Ok(result)
                    //},

                    PsycParseListRC::PSYC_PARSE_LIST_TYPE => (),

                    _error => {
                        return Err(mem::transmute(_error))
                    },
                }
            }
        }
    }
}

impl<'a> Parser<'a> for PsycListParser {
    fn set_buffer(&mut self, buffer: &'a [u8]) {
        let state_ptr = &mut self.state as *mut PsycParseListState;
        let buffer_ptr = buffer.as_ptr() as *const c_char;
        unsafe {
            psyc_parse_list_buffer_set(state_ptr, buffer_ptr, buffer.len())
        }
    }

    fn unparsed_position(&self) -> usize {
        self.state.cursor
    }

    fn unparsed_length(&self) -> usize {
        self.state.buffer.length - self.state.cursor
    }
}

impl<'a> PsycDictParser<'a> {
    /// Create a PsycDictParser
    pub fn new() -> Self {
        let mut state: PsycParseDictState;
        unsafe {
            state = mem::uninitialized();
            let state_ptr = &mut state as *mut PsycParseDictState;
            psyc_parse_dict_state_init(state_ptr)
        }
        PsycDictParser {
            state: state,
            parsed_key: None,
            buffer: None,
            cursor: 0
        }
    }

    /// Parse the buffer previously set by set_buffer. Call repeatedly until the
    /// result is PsycDictParserResult::Complete or a PsycDictParserError
    pub fn parse(&mut self)
                 -> Result<PsycDictParserResult<'a>, PsycDictParserError> {
        if self.buffer == None {
            return Err(PsycDictParserError::NoBuffer)
        }
        if self.buffer.unwrap().is_empty() {
            return Ok(PsycDictParserResult::Complete);
        }
        let state_ptr = &mut self.state as *mut PsycParseDictState;
        let mut list_type: PsycString;
        let mut element: PsycString;
        unsafe {
            list_type = mem::uninitialized();
            element = mem::uninitialized();
            let list_type_ptr = &mut list_type as *mut PsycString;
            let element_ptr = &mut element as *mut PsycString;
            loop {
                let parse_result = psyc_parse_dict(state_ptr, list_type_ptr, element_ptr);
                println!("parse_result: {:?}", parse_result);
                println!("cursor: {}", self.state.cursor);
                self.cursor = self.state.cursor;
                match parse_result {
                    PsycParseDictRC::PSYC_PARSE_DICT_END =>
                        return Ok(PsycDictParserResult::Complete),

                    PsycParseDictRC::PSYC_PARSE_DICT_INSUFFICIENT =>
                        return Ok(PsycDictParserResult::InsufficientData),

                    PsycParseDictRC::PSYC_PARSE_DICT_KEY => {
                        let key = util::cstring_to_slice(element.data, element.length);
                        self.parsed_key = Some(key) 
                    },

                    PsycParseDictRC::PSYC_PARSE_DICT_KEY_START => {
                        self.cursor = element.data as usize -
                                      self.buffer.unwrap().as_ptr() as usize;
                        return Ok(PsycDictParserResult::InsufficientData)
                    },

                    PsycParseDictRC::PSYC_PARSE_DICT_KEY_CONT => {
                        return Ok(PsycDictParserResult::InsufficientData)
                    },

                    PsycParseDictRC::PSYC_PARSE_DICT_KEY_END => {
                        let end_index = self.cursor + element.length;
                        let key = &self.buffer.unwrap()[self.cursor .. end_index];
                        self.parsed_key = Some(key)
                    },

                    //PsycParseDictRC::PSYC_PARSE_DICT_VALUE => {
                    //    let result = PsycDictParserResult::DictEntry {
                    //        key: self.parsed_key.unwrap(),
                    //        value: util::cstring_to_slice(element.data, element.length)
                    //    };
                    //    return Ok(result)
                    //},

                    PsycParseDictRC::PSYC_PARSE_DICT_VALUE |
                    PsycParseDictRC::PSYC_PARSE_DICT_VALUE_LAST => {
                        //let result: PsycDictParserResult;
                        //// FIXME: workaround
                        //if ! self.finished {
                        //    result = PsycDictParserResult::DictEntry {
                        //        key: self.parsed_key.unwrap(),
                        //        value: util::cstring_to_slice(element.data, element.length)
                        //    };
                        //    self.finished = true
                        //} else {
                        //    result = PsycDictParserResult::Complete;
                        //}
                        let result = PsycDictParserResult::DictEntry {
                            key: self.parsed_key.unwrap(),
                            value: util::cstring_to_slice(element.data, element.length)
                        };
                        return Ok(result)
                    },

                    PsycParseDictRC::PSYC_PARSE_DICT_VALUE_START => {
                        let result = PsycDictParserResult::DictEntryStart {
                            key: self.parsed_key.unwrap(),
                            value_part: util::cstring_to_slice(element.data, element.length)
                        };
                        return Ok(result)
                    },

                    PsycParseDictRC::PSYC_PARSE_DICT_VALUE_CONT => {
                        let result = PsycDictParserResult::DictEntryCont {
                            value_part: util::cstring_to_slice(element.data, element.length)
                        };
                        return Ok(result)
                    },

                    PsycParseDictRC::PSYC_PARSE_DICT_VALUE_END => {
                        let result = PsycDictParserResult::DictEntryEnd {
                            value_part: util::cstring_to_slice(element.data, element.length)
                        };
                        return Ok(result)
                    },

                    PsycParseDictRC::PSYC_PARSE_DICT_TYPE => (),
                    
                    _error => {
                        return Err(mem::transmute(_error))
                    },
                }
            }
        }
    }
}

impl<'a> Parser<'a> for PsycDictParser<'a> {
    fn set_buffer(&mut self, buffer: &'a [u8]) {
        self.buffer = Some(buffer);
        let state_ptr = &mut self.state as *mut PsycParseDictState;
        let buffer_ptr = buffer.as_ptr() as *const c_char;
        unsafe {
            psyc_parse_dict_buffer_set(state_ptr, buffer_ptr, buffer.len());
        }
    }

    fn unparsed_position(&self) -> usize {
        self.cursor
    }

    fn unparsed_length(&self) -> usize {
        self.state.buffer.length - self.cursor
    }
}

pub trait Parser<'a> {
    /// Set a buffer of raw bytes for parsing
    fn set_buffer(&mut self, buffer: &'a [u8]);

    /// copies the remaining unparsed bytes to the beginning of the given buffer.
    /// Returns the number of copied bytes. Must be called when parse() returned
    /// InsufficientData as Result.
    fn copy_unparsed_into_buffer(&self, buffer: &'a mut [u8]) -> usize {
        let unparsed_pos = self.unparsed_position();
        let unparsed_len = self.unparsed_length();
        if unparsed_pos != 0 {
            let copy_pos_second = unparsed_pos - unparsed_len;
            let (part1, part2) = buffer.split_at_mut(unparsed_len);
            part1.copy_from_slice(&part2[copy_pos_second .. copy_pos_second + unparsed_len]);
        }
        unparsed_len
    }

    fn unparsed_position(&self) -> usize;
    fn unparsed_length(&self) -> usize;
}
