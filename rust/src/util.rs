use packet_types::{RawPsycList, PsycRenderRC};
use std::slice;
use std::os::raw::c_char;

extern "C" {
    fn psyc_render_list(list: *const RawPsycList, buffer: *mut c_char, buflen: usize) -> PsycRenderRC;
}

pub unsafe fn cstring_to_slice<'a>(cstring: *const c_char, length: usize) -> &'a[u8] {
    slice::from_raw_parts(cstring as *const u8, length)
}

pub unsafe fn render_list(list: &RawPsycList) -> Vec<u8> {
    let mut buffer: Vec<u8> = Vec::with_capacity(list.length);
    buffer.set_len(list.length);
    let buffer_ptr = buffer.as_ptr() as *mut c_char;
    let list_ptr = list as *const RawPsycList;
    let _ = psyc_render_list(list_ptr, buffer_ptr, list.length);
    buffer
}
