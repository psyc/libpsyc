extern crate psyc;

use psyc::*;
use psyc::packet_types::*;

#[test]
fn test_create_render() {
    let r1 = PsycModifier::new(PsycOperator::PSYC_OPERATOR_SET,
                               "_target",
                               "psyc://ve.symlynx.com/@blog".as_bytes());

    let r2 = PsycModifier::new(PsycOperator::PSYC_OPERATOR_SET,
                               "_tag",
                               "666666".as_bytes());

    let e1 = PsycModifier::new(PsycOperator::PSYC_OPERATOR_SET,
                               "_nick",
                               "lurchi".as_bytes());

    let routing_modifiers = vec![r1, r2];
    let entity_modifiers = vec![e1];
    let data = vec![];

    let packet = PsycPacket::new(&routing_modifiers,
                                 &entity_modifiers,
                                 "_request_context_enter",
                                 &data,
                                 PsycStateOp::PSYC_STATE_NOOP);

    let expected = ":_target\tpsyc://ve.symlynx.com/@blog\n:_tag\t666666\n\n:_nick\tlurchi\n_request_context_enter\n|\n".as_bytes().to_vec();

    let rendered_packet = packet.render();

    assert_eq!(rendered_packet, Ok(expected))
}

#[test]
fn test_list() {
    let r1 = PsycModifier::new(PsycOperator::PSYC_OPERATOR_SET,
                               "_target",
                               "psyc://ve.symlynx.com/@blog".as_bytes());

    let list = PsycList::from_strings(&["str1", "str2", "str3"]);

    let e1 = PsycModifier::with_list_value(PsycOperator::PSYC_OPERATOR_SET,
                                           "_list_test",
                                           &list);

    let routing_modifiers = vec![r1];
    let entity_modifiers = vec![e1];
    let data = vec![];

    let packet = PsycPacket::new(&routing_modifiers,
                                 &entity_modifiers,
                                 "",
                                 &data,
                                 PsycStateOp::PSYC_STATE_NOOP);

    let expected = ":_target\tpsyc://ve.symlynx.com/@blog\n34\n:_list_test 18\t| str1| str2| str3\n|\n".as_bytes().to_vec();

    let rendered_packet = packet.render();

    assert_eq!(rendered_packet, Ok(expected));
}

#[test]
fn test_dict() {
    let r1 = PsycModifier::new(PsycOperator::PSYC_OPERATOR_SET,
                               "_target",
                               "psyc://ve.symlynx.com/@blog".as_bytes());
 
    let dict = PsycDict::from_strings(&[("key1", "value1"), 
                                        ("key2", "value2"),
                                        ("key3", "value3")]);

    let e1 = PsycModifier::with_dict_value(PsycOperator::PSYC_OPERATOR_SET,
                                           "_dict_test",
                                           &dict);

    let routing_modifiers = vec![r1];
    let entity_modifiers = vec![e1];
    let data = vec![];

    let packet = PsycPacket::new(&routing_modifiers,
                                 &entity_modifiers,
                                 "",
                                 &data,
                                 PsycStateOp::PSYC_STATE_NOOP);

    let expected = ":_target\tpsyc://ve.symlynx.com/@blog\n58\n:_dict_test 42\t{ key1} value1{ key2} value2{ key3} value3\n|\n".as_bytes().to_vec();

    let rendered_packet = packet.render();   

    //println!("rendered: {}", String::from_utf8(rendered_packet.unwrap()).unwrap());

    assert_eq!(rendered_packet, Ok(expected));
}
