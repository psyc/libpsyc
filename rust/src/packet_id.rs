use packet_types::{RawPsycList, PsycElem};
use parser::{PsycListParser, PsycListParserResult};
use util;
use std::mem;
use std::ptr;
use std::os::raw::c_char;

extern "C" {
    fn psyc_packet_id(list: *mut RawPsycList,
                      elems: *mut PsycElem,
                      context: *const c_char,
                      contextlen: usize,
                      source: *const c_char,
                      sourcelen: usize,
                      target: *const c_char,
                      targetlen: usize,
                      counter: *const c_char,
                      counterlen: usize,
                      fragment: *const c_char,
                      fragmentlen: usize);
}

#[derive(PartialEq, Debug)]
pub struct PacketId<'a> {
    pub context: Option<&'a [u8]>,
    pub source: Option<&'a [u8]>,
    pub target: Option<&'a [u8]>,
    pub counter: Option<&'a [u8]>,
    pub fragment: Option<&'a [u8]>
}

impl<'a> PacketId<'a> {
    pub fn from_bytes(bytes: &'a [u8]) -> Option<Self> {
        let mut parsed: Vec<Option<&'a [u8]>> = vec![None; 5];
        let mut parser = PsycListParser::new();
        let mut parsing_error = false;
        for slice in &mut parsed {
            match parser.parse(bytes) {
                Ok(PsycListParserResult::ListElement {value: v}) => *slice = Some(v),
                _ => parsing_error = true
            }
        }
        if parsing_error {
            None
        } else {
            let result = PacketId {
                context: parsed[0],
                source: parsed[1],
                target: parsed[2],
                counter: parsed[3],
                fragment: parsed[4]
            };
            Some(result)
        }
    }

    pub fn render(&self) -> Vec<u8> {
        unsafe {
            let get_ptr = |slice: Option<&'a [u8]>| {
                match slice {
                    None => ptr::null::<c_char>(),
                    Some(s) => s.as_ptr() as *const c_char
                }
            };

            let get_len = |slice: Option<&'a [u8]>| {
                match slice {
                    None => 0,
                    Some(s) => s.len()
                }
            };

            let mut list: RawPsycList = mem::uninitialized();
            let mut elements: Vec<PsycElem> = Vec::with_capacity(5);
            let list_ptr = &mut list as *mut RawPsycList;
            let elements_ptr = elements.as_mut_ptr();
            
            psyc_packet_id(list_ptr,
                           elements_ptr,
                           get_ptr(self.context),
                           get_len(self.context),
                           get_ptr(self.source),
                           get_len(self.source),
                           get_ptr(self.target),
                           get_len(self.target),
                           get_ptr(self.counter),
                           get_len(self.counter),
                           get_ptr(self.fragment),
                           get_len(self.fragment)); 

            util::render_list(&list)   
        }
    }
}
