extern crate psyc;

use psyc::*;
use psyc::packet_types::*;

#[test]
fn test_packet_id() {
    let routing_modifiers = vec![PsycModifier::new(PsycOperator::PSYC_OPERATOR_SET,
                                                   "_source",
                                                   b"psyc://ve.symlynx.com/~alice"),
                                 PsycModifier::new(PsycOperator::PSYC_OPERATOR_SET,
                                                   "_target",
                                                   b"psyc://ve.symlynx.com/~bob"),
                                 PsycModifier::new(PsycOperator::PSYC_OPERATOR_SET,
                                                   "_counter",
                                                   b"42")];

    let entity_modifiers = vec![];

    let data = vec![];

    let packet = PsycPacket::new(&routing_modifiers,
                                 &entity_modifiers,
                                 "",
                                 &data,
                                 PsycStateOp::PSYC_STATE_NOOP);

    let expected_id = PacketId {
        context: None,
        source: Some(b"psyc://ve.symlynx.com/~alice"),
        target: Some(b"psyc://ve.symlynx.com/~bob"),
        counter: Some(b"42"),
        fragment: None
    };

    assert_eq!(packet.packet_id(), expected_id);

    let expected_rendered = b"|| psyc://ve.symlynx.com/~alice| psyc://ve.symlynx.com/~bob| 42|";

   assert_eq!(packet.packet_id().render(), expected_rendered.to_vec());
}
