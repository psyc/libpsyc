use text_types::*;
use types::PsycString;
use method_types::PsycMethod;
use util;
use std::os::raw::c_char;
use std::os::raw::c_void;
use std::ptr;
use std::mem;
use std::marker::PhantomData;

extern "C" {
    fn psyc_text_state_init(state: *mut PsycTextState,
                            tmpl: *const c_char,
                            tmplen: usize,
                            buffer: *mut c_char,
                            buflen: usize);

    fn psyc_text_state_init_custom(state: *mut PsycTextState,
                                   tmpl_: *const c_char,
                                   tmplen: usize,
                                   buffer: *mut c_char,
                                   buflen: usize,
                                   ope: *const c_char,
                                   opelen: usize,
                                   clo: *const c_char,
                                   clolen: usize);

    fn psyc_text_buffer_set(state: *mut PsycTextState,
                            buffer: *mut c_char,
                            length: usize);

    fn psyc_text_bytes_written(state: *const PsycTextState) -> usize;

    fn psyc_text(state: *mut PsycTextState,
                 get_value: extern fn(*const c_void,
                                      *const c_char,
                                      usize,
                                      *mut PsycString)
                                      -> PsycTextValueRC,
                 get_value_cls: *const c_void)
                 -> PsycTextRC;

    fn psyc_template(mc: PsycMethod, len: *mut usize) -> *const c_char;
}

extern "C" fn callback(cls: *const c_void,
                       name: *const c_char,
                       namelen: usize,
                       value: *mut PsycString)
                       -> PsycTextValueRC {
    let get_value: &&for<'a> Fn(&'a [u8]) -> Option<&[u8]> = unsafe {
        mem::transmute(cls)
    };
    let name_slice = unsafe {
        util::cstring_to_slice(name, namelen)
    };
    match get_value(name_slice) {
        None => PsycTextValueRC::PSYC_TEXT_VALUE_NOT_FOUND,
        Some(val) => {
            let mut v: &mut PsycString = unsafe {&mut *value};
            v.data = val.as_ptr() as *const c_char;
            v.length = val.len();
            PsycTextValueRC::PSYC_TEXT_VALUE_FOUND
        }
    }
}

pub struct Text<'a> {
    state: PsycTextState,
    phantom_template: PhantomData<&'a Vec<u8>>,
}

impl<'a> Text<'a> {
    pub fn new(template: &'a [u8]) -> Self {
        let mut state: PsycTextState;
        let template_ptr = template.as_ptr() as *const c_char;
        unsafe {
            state = mem::uninitialized();
            let state_ptr = &mut state as *mut PsycTextState;
            psyc_text_state_init(state_ptr,
                                 template_ptr,
                                 template.len(),
                                 ptr::null_mut(),
                                 0);
        }
        Text {
            state: state,
            phantom_template: PhantomData,
        }
    }

    pub fn substitute_variables<'b, F>(&mut self,
                                   get_value: F,
                                   buffer: &'b mut [u8])
                                   -> SubstitutionResult 
        where F: Fn(&'a [u8]) -> Option<&[u8]> {
        let state_ptr = &mut self.state as *mut PsycTextState;
        let buffer_ptr = buffer.as_mut_ptr() as *mut c_char;
        let trait_obj: &Fn(&'a [u8]) -> Option<&[u8]> = &get_value;
        let trait_obj_ptr: *const c_void = unsafe {mem::transmute(&trait_obj)};
        unsafe {
            psyc_text_buffer_set(state_ptr, buffer_ptr, buffer.len());
            let result = psyc_text(state_ptr, callback, trait_obj_ptr);
            match result {
                PsycTextRC::PSYC_TEXT_NO_SUBST => SubstitutionResult::NoSubstitution,
                PsycTextRC::PSYC_TEXT_COMPLETE => SubstitutionResult::Complete {
                    bytes_written: psyc_text_bytes_written(state_ptr)
                },
                PsycTextRC::PSYC_TEXT_INCOMPLETE => SubstitutionResult::Incomplete {
                    bytes_written: psyc_text_bytes_written(state_ptr)
                }
            }
        }
    }

    pub fn template(method: PsycMethod) -> &'static [u8] {
        let mut result: PsycString;
        unsafe {
            result = mem::uninitialized();
            result.data = psyc_template(method, &mut result.length as *mut usize);
            util::cstring_to_slice(result.data, result.length)
        }
    }
}
