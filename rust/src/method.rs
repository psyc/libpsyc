use method_types::{PsycMethod, PsycMethodFlags, MethodInfo};
use keyword::Keyword;
use std::mem;
use std::os::raw::{c_char, c_uint};

extern "C" {
    fn psyc_method(method: *const c_char,
                   methodlen: usize,
                   family: *mut PsycMethod,
                   flag: *mut c_uint)
                   -> PsycMethod;
}

pub struct Method<'a> {
    pub method: &'a [u8]
}

impl<'a> Keyword for Method<'a> {
    fn keyword(&self) -> &[u8] {
        self.method
    }
}

impl<'a> Method<'a> {
    pub fn lookup(&self) -> MethodInfo {
        let method_ptr = self.method.as_ptr() as *const c_char;
        let lookup_result: PsycMethod;
        let mut family: PsycMethod;
        let mut flags: PsycMethodFlags;
        unsafe {
            family = mem::uninitialized();
            flags = mem::uninitialized();
            let flags_ptr = &mut flags as *mut PsycMethodFlags as *mut c_uint;
            lookup_result = psyc_method(method_ptr,
                                        self.method.len(), 
                                        &mut family as *mut PsycMethod,
                                        flags_ptr)
        }
        MethodInfo {
            lookup_result: lookup_result,
            family: family,
            flags: flags
        }
    }
}
