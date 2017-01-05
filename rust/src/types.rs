use std::os::raw::c_char;

/// Return code: OK/error.
#[repr(C)]
pub enum PsycRC {
    PSYC_OK = 1,
    PSYC_ERROR = -1,
}

#[repr(C)]
pub enum PsycBool {
    PSYC_FALSE = 0,
    PSYC_TRUE = 1,
}

#[derive(Debug)]
#[repr(C)]
pub struct PsycString {
    pub length: usize,
    pub data: *const c_char
}

//#[derive(Debug)]
//#[repr(C)]
//pub struct MutablePsycString {
//    pub length: usize,
//    pub data: *mut c_char
//}

impl PsycBool {
    pub fn to_bool(self) -> bool {
        match self {
            PsycBool::PSYC_FALSE => false,
            PsycBool::PSYC_TRUE => true
        }
    }
}

#[derive(Copy, Clone, Debug, PartialEq)]
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


