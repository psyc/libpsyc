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

impl PsycBool {
    pub fn to_bool(self) -> bool {
        match self {
            PsycBool::PSYC_FALSE => false,
            PsycBool::PSYC_TRUE => true
        }
    }
}
