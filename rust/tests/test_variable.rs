extern crate psyc;
use psyc::*;

#[test]
fn test_routing_variable() {
    let r = RoutingVariable {variable: b"_amount_fragments"};

    assert_eq!(r.lookup(), PsycRoutingVar::PSYC_RVAR_AMOUNT_FRAGMENTS);
    assert_eq!(r.datatype(), PsycType::PSYC_TYPE_AMOUNT);
    assert_eq!(r.is_list(), false);
}

#[test]
fn test_entity_variable() {
    let e = EntityVariable {variable: b"_nick_family"};

    assert_eq!(e.datatype(), PsycType::PSYC_TYPE_NICK);
    assert!(! e.is_list());
}

#[test]
fn test_empty() {
    let r = RoutingVariable {variable: b""};

    assert_eq!(r.lookup(), PsycRoutingVar::PSYC_RVAR_UNKNOWN);
    assert_eq!(r.datatype(), PsycType::PSYC_TYPE_UNKNOWN);
    assert!(! r.is_list());
}

#[test]
fn test_inherits() {
    let r = RoutingVariable {variable: b"_target_relay"};

    assert!(r.inherits(b"_target"));
    assert!(! r.inherits(b""));
}

#[test]
fn test_matches() {
    let r = RoutingVariable {variable: b"_target_relay"};

    assert!(r.matches(b"_relay"));
    assert!(! r.matches(b""));
}
