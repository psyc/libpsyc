extern crate psyc;

use psyc::*;

#[test]
fn test_create_render() {
    let r1 = PsycModifier::new(PsycOperator::PSYC_OPERATOR_SET,
                               b"_target",
                               b"psyc://ve.symlynx.com/@blog");

    let r2 = PsycModifier::new(PsycOperator::PSYC_OPERATOR_SET,
                               b"_tag",
                               b"666666");

    let e1 = PsycModifier::new(PsycOperator::PSYC_OPERATOR_SET,
                               b"_nick",
                               b"lurchi");

    let routing_modifiers = vec![r1, r2];
    let entity_modifiers = vec![e1];
    let data = vec![];

    let packet = PsycPacket::new(&routing_modifiers,
                                 &entity_modifiers,
                                 b"_request_context_enter",
                                 &data,
                                 PsycStateOp::PSYC_STATE_NOOP);

    let expected = ":_target\tpsyc://ve.symlynx.com/@blog\n:_tag\t666666\n\n:_nick\tlurchi\n_request_context_enter\n|\n".as_bytes().to_vec();

    let rendered_packet = packet.render();

    assert_eq!(rendered_packet, Ok(expected))
}

#[test]
fn test_list() {
    let r1 = PsycModifier::new(PsycOperator::PSYC_OPERATOR_SET,
                               b"_target",
                               b"psyc://ve.symlynx.com/@blog");

    let list = PsycList::from_strings(&["str1", "str2", "str3"]);

    let e1 = PsycModifier::with_list_value(PsycOperator::PSYC_OPERATOR_SET,
                                           b"_list_test",
                                           &list);

    let routing_modifiers = vec![r1];
    let entity_modifiers = vec![e1];
    let data = vec![];

    let packet = PsycPacket::new(&routing_modifiers,
                                 &entity_modifiers,
                                 b"",
                                 &data,
                                 PsycStateOp::PSYC_STATE_NOOP);

    let expected = ":_target\tpsyc://ve.symlynx.com/@blog\n34\n:_list_test 18\t| str1| str2| str3\n|\n".as_bytes().to_vec();

    let rendered_packet = packet.render();

    assert_eq!(rendered_packet, Ok(expected));
}
