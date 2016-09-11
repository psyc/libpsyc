use types::PsycBool;
use variable_types::*;
use keyword::Keyword;
use std::os::raw::c_char;

extern "C" {
    fn psyc_var_routing(name: *const c_char, len: usize) -> PsycRoutingVar;
    fn psyc_var_type(name: *const c_char, len: usize) -> PsycType;
    fn psyc_var_is_list(name: *const c_char, len: usize) -> PsycBool;
}

pub struct RoutingVariable<'a> {
    pub variable: &'a [u8]
}

pub struct EntityVariable<'a> {
    pub variable: &'a [u8]
}

impl<'a> Variable for RoutingVariable<'a> {
    fn variable(&self) -> &[u8] {
        self.variable
    }
}

impl<'a> Variable for EntityVariable<'a> {
    fn variable(&self) -> &[u8] {
        self.variable
    }
}

impl<'a> Keyword for RoutingVariable<'a> {
    fn keyword(&self) -> &[u8] {
        self.variable
    }
}

impl<'a> Keyword for EntityVariable<'a> {
    fn keyword(&self) -> &[u8] {
        self.variable
    }
}

impl<'a> RoutingVariable<'a> {
    pub fn lookup(&self) -> PsycRoutingVar {
        let variable_ptr = self.variable().as_ptr() as *const c_char;
        unsafe {
            psyc_var_routing(variable_ptr, self.variable().len())
        }
    }
}

pub trait Variable {
    fn variable(&self) -> &[u8];

    fn datatype(&self) -> PsycType {
        let variable_ptr = self.variable().as_ptr() as *const c_char;
        unsafe {
            psyc_var_type(variable_ptr, self.variable().len())
        }
    }

    fn is_list(&self) -> bool {
        let variable_ptr = self.variable().as_ptr() as *const c_char;
        unsafe {
            psyc_var_is_list(variable_ptr, self.variable().len()).to_bool()
        }
    }
}
