extern crate psyc;

use psyc::*;

#[test]
fn test_substitute() {
    let template = b"Your hello is [_hello]";
    let mut t = Text::new(template);

    let mut buffer = vec![0; 100];

    let result = t.substitute_variables(|name| {
        println!("in callback, name: {:?}", name);
        match name {
            b"_hello" => Some(b"hi"),
            _ => None
        }},
        &mut buffer);

    assert_eq!(result, SubstitutionResult::Complete {bytes_written: 16});
    assert_eq!(&buffer[.. 16], b"Your hello is hi");
}

#[test]
fn test_empty() {
    let mut t = Text::new(b"");

    let mut buffer = Vec::new();

    assert_eq!(t.substitute_variables(|_name| {Some(b"hi")}, &mut buffer),
               SubstitutionResult::NoSubstitution);
}

#[test]
fn test_incomplete() {
    let template = b"Your hello is [_hello]";
    let mut t = Text::new(template);

    let mut buffer = vec![0; 10];

    assert_eq!(t.substitute_variables(|_name| {Some(b"hi")}, &mut buffer),
               SubstitutionResult::Incomplete {bytes_written: 0});

    buffer.resize(15, 0);

    assert_eq!(t.substitute_variables(|_name| {Some(b"hi")}, &mut buffer),
               SubstitutionResult::Incomplete {bytes_written: 14});

    buffer.resize(16, 0);
    assert_eq!(t.substitute_variables(|_name| {Some(b"hi")}, &mut buffer[14 ..]),
               SubstitutionResult::Complete {bytes_written: 2});

    assert_eq!(&buffer, b"Your hello is hi");
}

#[test]
fn test_template() {
    let template = Text::template(PsycMethod::PSYC_MC_REQUEST_CONTEXT_ENTER);
    let expected: &'static [u8] = b"[_source] asks for your permission to enter [_context]";

    assert_eq!(template, expected);
}
