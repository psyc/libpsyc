use types::PsycString;

#[repr(C)]
pub enum PsycTextRC {
    /// No substitution was made, nothing was written to the buffer.
    PSYC_TEXT_NO_SUBST = -1,
    /// Text template parsing & rendering complete.
    PSYC_TEXT_COMPLETE = 0,
    /// Text template parsing & rendering is incomplete, because the buffer was too
    /// small. Another call is required to this function after setting a new buffer.
    PSYC_TEXT_INCOMPLETE = 1,
}

#[repr(C)]
pub enum PsycTextValueRC {
    /// Value not found, don't substitute anything.
    PSYC_TEXT_VALUE_NOT_FOUND = -1,
    /// Value found, substitute contents of the value variable.
    PSYC_TEXT_VALUE_FOUND = 0,
}

#[repr(C)]
pub struct PsycTextState {
    cursor: usize,		///< current position in the template
    written: usize,		///< number of bytes written to buffer
    tmpl: PsycString,		///< input buffer with text template to parse
    buffer: PsycString,		///< output buffer for rendered text
    open: PsycString,
    close: PsycString,
}

#[derive(Debug, PartialEq)]
pub enum SubstitutionResult {
    NoSubstitution,
    Complete {
        bytes_written: usize 
    },
    Incomplete {
        bytes_written: usize
    }
}
