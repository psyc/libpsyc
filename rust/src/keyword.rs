use std::os::raw::{c_char, c_int};

extern "C" {
    fn psyc_inherits(sho: *const c_char,
                     slen: usize,
                     lon: *const c_char,
                     llen: usize)
                     -> c_int;
    fn psyc_matches(sho: *const c_char,
                    slen: usize,
                    lon: *const c_char,
                    llen: usize)
                    -> c_int;
}

pub trait Keyword {
    fn keyword(&self) -> &[u8];

    fn inherits(&self, other: &[u8]) -> bool {
        let keyword_ptr = self.keyword().as_ptr() as *const c_char;
        let other_ptr = other.as_ptr() as *const c_char;
        let result = unsafe {
            psyc_inherits(other_ptr, other.len(), keyword_ptr, self.keyword().len())
        };
        result == 0
    }

    fn matches(&self, other: &[u8]) -> bool {
        let keyword_ptr = self.keyword().as_ptr() as *const c_char;
        let other_ptr = other.as_ptr() as *const c_char;
        let result = unsafe {
            psyc_matches(other_ptr, other.len(), keyword_ptr, self.keyword().len())
        };
        result == 0
    }
}
