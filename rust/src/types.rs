use std::os::raw::c_char;

/// Return code: OK/error.
#[repr(C)]
pub enum PsycRC {
    PSYC_OK = 1,
    PSYC_ERROR = -1,
}

#[derive(Debug)]
#[repr(C)]
pub struct PsycString {
    pub length: usize,
    pub data: *const c_char
}


