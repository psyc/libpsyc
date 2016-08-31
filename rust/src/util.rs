use std::slice;
use std::os::raw::c_char;

pub unsafe fn cstring_to_slice<'a>(cstring: *const c_char, length: usize) -> &'a[u8] {
    slice::from_raw_parts(cstring as *const u8, length)
}
