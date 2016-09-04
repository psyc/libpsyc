use types::*;
use packet_types::*;
use packet_id::*;
use util;
use std::mem;
use std::ptr;
use std::os::raw::c_char;

extern "C" {
    /// functions from packet.h
    fn psyc_num_length(n: usize) -> usize;
    fn psyc_modifier_length_check(m: *const RawPsycModifier) -> PsycModifierFlag;
    fn psyc_modifier_init(m: *mut RawPsycModifier,
                          oper: PsycOperator,
                          name: *const c_char,
                          namelen: usize,
                          value: *const c_char,
                          valuelen: usize,
                          flag: PsycModifierFlag);

    fn psyc_elem_length_check(value: *const PsycString, end: c_char) -> PsycElemFlag;
    fn psyc_elem_length(elem: *const PsycElem) -> usize;
    fn psyc_list_length_set(list: *mut RawPsycList) -> usize;
    fn psyc_modifier_length(m: *const RawPsycModifier) -> usize;
    fn psyc_packet_length_check(p: *const RawPsycPacket) -> PsycPacketFlag;
    fn psyc_packet_length_set(p: *mut RawPsycPacket) -> usize;
    fn psyc_list_init(list: *mut RawPsycList, elems: *const PsycElem, num_elems: usize);
    fn psyc_packet_init(packet: *mut RawPsycPacket,
                        routing: *const RawPsycModifier,
                        routinglen: usize,
                        entity: *const RawPsycModifier,
                        entitylen: usize,
                        method: *const c_char,
                        methodlen: usize,
                        data: *const c_char,
                        datalen: usize,
                        stateop: c_char,
                        flag: PsycPacketFlag);

    fn psyc_packet_init_raw(packet: *mut RawPsycPacket,
                            routing: *const RawPsycModifier,
                            routinglen: usize,
                            content: *const c_char,
                            contentlen: usize,
                            flag: PsycPacketFlag);
   
    /// functions from render.h
    fn psyc_render(packet: *const RawPsycPacket, buffer: *mut c_char, buflen: usize) -> PsycRenderRC;
    fn psyc_render_modifier(modifier: *const RawPsycModifier, buffer: *mut c_char) -> usize;
    fn psyc_render_elem(elem: *const PsycElem, buffer: *mut c_char, buflen: usize) -> PsycRenderRC;
}

pub struct PsycList {
    rendered_list: Vec<u8>
}

pub struct PsycModifier<'a> {
    name: &'a str,
    value: &'a [u8],
    operator: PsycOperator,
}

pub struct PsycPacket<'a> {
    raw_packet: RawPsycPacket,
    routing_modifiers: &'a [PsycModifier<'a>],
    entity_modifiers: &'a [PsycModifier<'a>],
    raw_routing_modifiers: Vec<RawPsycModifier>,
    raw_entity_modifiers: Vec<RawPsycModifier>,
    method: &'a str,
    body: &'a [u8]
}

#[repr(C)]
#[derive(Debug, PartialEq)]
pub enum PsycRenderError {
    MethodMissing = PsycRenderRC::PSYC_RENDER_ERROR_METHOD_MISSING as _,
    ModifierNameMissing = PsycRenderRC::PSYC_RENDER_ERROR_MODIFIER_NAME_MISSING as _,
    GenericError = PsycRenderRC::PSYC_RENDER_ERROR as _
}

impl PsycList {
    /// Construct a PsycList from a list of byte lists
    pub fn new(list: &[&[u8]]) -> Self {
        let mut psyc_list: RawPsycList;
        let elements: Vec<PsycElem>;
        let buffer: Vec<u8>;
        unsafe {
            psyc_list = mem::uninitialized();
            let psyc_list_ptr = &mut psyc_list as *mut RawPsycList;
            elements = list.iter().map(|e| make_psyc_elem(&e)).collect();
            let elements_ptr = elements.as_ptr() as *const PsycElem;
            psyc_list_init(psyc_list_ptr, elements_ptr, list.len());
            buffer = util::render_list(&psyc_list)
        }
        PsycList {
            rendered_list: buffer
        }
    }

    /// Construct a PsycList from a list of strings (comfort function)
    pub fn from_strings(list: &[&str]) -> Self {
        let list_slices: Vec<&[u8]> = list.iter().map(|e| e.as_bytes()).collect();
        Self::new(&list_slices)
    }
}

impl<'a> PsycModifier<'a> {
    /// construct a PsycModifier
    pub fn new(operator: PsycOperator, name: &'a str, value: &'a [u8]) -> Self {
        PsycModifier {name: name, value: value, operator: operator}
    }

    /// construct a PsycModifier with string value (comfort function)
    pub fn with_string_value(operator: PsycOperator,
                             name: &'a str,
                             value: &'a str)
                             -> Self {
        PsycModifier {
            name: name,
            value: value.as_bytes(),
            operator: operator
        }
    }

    /// construct a PsycModifier with list value
    pub fn with_list_value(operator: PsycOperator,
                          name: &'a str,
                          value: &'a PsycList)
                          -> Self {
        PsycModifier {
            name: name,
            value: &value.rendered_list,
            operator: operator
        }
    }
}

impl<'a> PsycPacket<'a> {
    ///
    pub fn new(routing_modifiers: &'a [PsycModifier],
               entity_modifiers: &'a [PsycModifier],
               method: &'a str,
               data: &'a [u8],
               state_operator: PsycStateOp)
               -> Self {
        let mut packet: RawPsycPacket;
        let raw_routing_modifiers: Vec<RawPsycModifier>;
        let raw_entity_modifiers: Vec<RawPsycModifier>;
        unsafe{
            packet = mem::uninitialized();
            let packet_ptr = &mut packet as *mut RawPsycPacket;
            raw_routing_modifiers = routing_modifiers.iter().map(
                |m| Self::make_raw_modifier(m, PsycModifierFlag::PSYC_MODIFIER_ROUTING)
            ).collect();
            raw_entity_modifiers = entity_modifiers.iter().map(
                |m| Self::make_raw_modifier(m, PsycModifierFlag::PSYC_MODIFIER_CHECK_LENGTH)
            ).collect();
            psyc_packet_init(packet_ptr,
                             raw_routing_modifiers.as_ptr() as *const RawPsycModifier,
                             raw_routing_modifiers.len(),
                             raw_entity_modifiers.as_ptr() as *const RawPsycModifier,
                             raw_entity_modifiers.len(),
                             method.as_ptr() as *const c_char,
                             method.len(),
                             data.as_ptr() as *const c_char,
                             data.len(),
                             state_operator as c_char,
                             PsycPacketFlag::PSYC_PACKET_CHECK_LENGTH);
        }
        PsycPacket {
            raw_packet: packet,
            routing_modifiers: routing_modifiers,
            entity_modifiers: entity_modifiers,
            raw_routing_modifiers: raw_routing_modifiers,
            raw_entity_modifiers: raw_entity_modifiers,
            method: method,
            body: data
        }
    }

    pub fn new_from_raw(routing_modifiers: &'a [PsycModifier],
                        content: &'a [u8]) -> Self {
        let mut packet: RawPsycPacket;
        let raw_routing_modifiers: Vec<RawPsycModifier>;
        unsafe {
            packet = mem::uninitialized();
            let packet_ptr = &mut packet as *mut RawPsycPacket;
            raw_routing_modifiers = routing_modifiers.iter().map(
                |m| Self::make_raw_modifier(m, PsycModifierFlag::PSYC_MODIFIER_ROUTING)
            ).collect();
            psyc_packet_init_raw(packet_ptr,
                                 raw_routing_modifiers.as_ptr() as *const RawPsycModifier,
                                 raw_routing_modifiers.len(),
                                 content.as_ptr() as *const c_char,
                                 content.len(),
                                 PsycPacketFlag::PSYC_PACKET_CHECK_LENGTH);
        }
        PsycPacket {
            raw_packet: packet,
            routing_modifiers: routing_modifiers,
            entity_modifiers: &[],
            raw_routing_modifiers: raw_routing_modifiers,
            raw_entity_modifiers: vec![],
            method: "",
            body: content
        }
    }

    /// the length of the rendered packet 
    pub fn length(&self) -> usize {
        self.raw_packet.length
    }

    ///
    pub fn render(&self) -> Result<Vec<u8>, PsycRenderError> {
        let raw_packet_ptr = & self.raw_packet as *const RawPsycPacket;
        let mut buffer = Vec::with_capacity(self.length());
        let buffer_ptr = buffer.as_mut_ptr() as *mut c_char;
        unsafe {
            buffer.set_len(self.length());
            let result = psyc_render(raw_packet_ptr, buffer_ptr, buffer.capacity());
            match result {
                PsycRenderRC::PSYC_RENDER_SUCCESS => Ok(buffer),
                _error => Err(mem::transmute(_error))
            }
        }
    }

    /// get the packet id
    pub fn packet_id(&self) -> PacketId<'a> {
        let get_value = |modifier: Option<&PsycModifier<'a>>| {
            match modifier {
                None => None,
                Some(m) => Some(m.value)
            }
        };

        let context = self.routing_modifiers.iter().find(|&r| r.name == "_context");
        let source = self.routing_modifiers.iter().find(|&r| r.name == "_source");
        let target = self.routing_modifiers.iter().find(|&r| r.name == "_target");
        let counter = self.routing_modifiers.iter().find(|&r| r.name == "_counter");

        PacketId {
            context: get_value(context),
            source: get_value(source),
            target: get_value(target),
            counter: get_value(counter),
            fragment: None
        }
    }

    fn make_raw_modifier(modifier: &'a PsycModifier,
                         flag: PsycModifierFlag)
                         -> RawPsycModifier {
        let mut raw_modifier: RawPsycModifier;
        unsafe {
            raw_modifier = mem::uninitialized();
            let raw_modifier_ptr = &mut raw_modifier as *mut RawPsycModifier;
            let name_ptr = modifier.name.as_ptr() as *const c_char;
            let value_ptr = modifier.value.as_ptr() as *const c_char;
            psyc_modifier_init(raw_modifier_ptr,
                               modifier.operator,
                               name_ptr,
                               modifier.name.len(), 
                               value_ptr,
                               modifier.value.len(),
                               flag);
        }
        raw_modifier
    }
}

unsafe fn make_psyc_elem(list_element: &[u8]) -> PsycElem {
    let list_element_ptr = list_element.as_ptr() as *const c_char;
    PsycElem {
        elem_type: PsycString {length: 0, data: ptr::null()},
        value: PsycString {length: list_element.len(), data: list_element_ptr},
        length: 0,
        flag: PsycElemFlag::PSYC_ELEM_CHECK_LENGTH
    }
}
