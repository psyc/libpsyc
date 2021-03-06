extern crate psyc;

use psyc::*;

#[test]
fn test_packet_id() {
    let routing_modifiers = vec![PsycModifier::new(PsycOperator::PSYC_OPERATOR_SET,
                                                   b"_source",
                                                   b"psyc://ve.symlynx.com/~alice"),
                                 PsycModifier::new(PsycOperator::PSYC_OPERATOR_SET,
                                                   b"_target",
                                                   b"psyc://ve.symlynx.com/~bob"),
                                 PsycModifier::new(PsycOperator::PSYC_OPERATOR_SET,
                                                   b"_counter",
                                                   b"42")];

    let entity_modifiers = vec![];

    let data = vec![];

    let packet = PsycPacket::new(&routing_modifiers,
                                 &entity_modifiers,
                                 b"",
                                 &data,
                                 PsycStateOp::PSYC_STATE_NOOP);

    let expected_id = PacketId {
        context: None,
        source: Some(b"psyc://ve.symlynx.com/~alice"),
        target: Some(b"psyc://ve.symlynx.com/~bob"),
        counter: Some(b"42"),
        fragment: None
    };

    let packet_id = packet.packet_id();

    assert_eq!(packet_id, expected_id);

    let expected_rendered = b"|| psyc://ve.symlynx.com/~alice| psyc://ve.symlynx.com/~bob| 42|";

    let rendered_id = packet_id.render();

   assert_eq!(rendered_id, expected_rendered.to_vec());

   let parsed_id = PacketId::from_bytes(&rendered_id);

   assert_eq!(parsed_id.unwrap(), packet_id);
}
