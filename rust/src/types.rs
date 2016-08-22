use std::os::raw::c_char;

#[repr(C)]
pub struct PsycString {
    pub length: usize,
    pub data: *const c_char
}


