extern crate psyc;

use psyc::*;

#[test]
fn test_person() {
    let uniform = Uniform::from_bytes(b"psyc://ve.symlynx.com:4404/~lurchi");

    let unwrapped = uniform.unwrap();
    let entity = unwrapped.entity();

    assert!(unwrapped.is_valid());
    assert_eq!(unwrapped.full(), "psyc://ve.symlynx.com:4404/~lurchi");
    assert_eq!(unwrapped.scheme(), PsycScheme::PSYC_SCHEME_PSYC);
    assert_eq!(unwrapped.host(), "ve.symlynx.com");
    assert_eq!(unwrapped.port(), Some(4404));
    assert_eq!(unwrapped.resource(), "~lurchi");

    assert_eq!(entity, PsycEntity::Person {name: "lurchi", channel: ""});
}

#[test]
fn test_place() {
    let uniform = Uniform::from_bytes(b"psyc://ve.symlynx.com/@_strange_place");

    let unwrapped = uniform.unwrap();

    assert!(unwrapped.is_valid());
    assert_eq!(unwrapped.resource(), "@_strange_place");
    assert_eq!(unwrapped.entity(),
               PsycEntity::Place {name: "_strange_place", channel: ""});
               
}

#[test]
fn test_host_only() {
    let uniform = Uniform::from_bytes(b"psyc://ve.symlynx.com");

    let unwrapped = uniform.unwrap();

    assert!(unwrapped.is_valid());
    assert_eq!(unwrapped.resource(), "");
    assert_eq!(unwrapped.entity(), PsycEntity::Root);
}

#[test]
fn test_unknown_resource() {
    let uniform = Uniform::from_bytes(b"psyc://ve.symlynx.com/%unknown#a");

    let unwrapped = uniform.unwrap();

    assert!(unwrapped.is_valid());
    assert_eq!(unwrapped.entity(),
               PsycEntity::Unknown {object: "%unknown", channel: "a"});
}

#[test]
fn test_channel() {
    let uniform = Uniform::from_bytes(b"psyc://ve.symlynx.com:4404/~lurchi#test");

    let unwrapped = uniform.unwrap();
    
    assert_eq!(unwrapped.entity(), PsycEntity::Person {name: "lurchi", channel: "test"});
}

#[test]
fn test_empty() {
    let uniform = Uniform::from_bytes(b"");
    assert_eq!(uniform.unwrap_err(),
               UniformParseError::PSYC_PARSE_UNIFORM_INVALID_SCHEME);
}
