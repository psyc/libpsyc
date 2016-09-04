extern crate psyc;
use psyc::parser::*;

#[test]
fn test_parse() {
    let test_data = "| element1| element2| element3".to_string().into_bytes();

    let expected1 = PsycListParserResult::ListElement {
        value: b"element1"
    };

    let expected2 = PsycListParserResult::ListElement {
        value: b"element2"
    };
    
    let expected3 = PsycListParserResult::ListElement {
        value: b"element3"
    };

    let expected4 = PsycListParserResult::Complete;

    let mut parser = PsycListParser::new();

    //parser.set_buffer(&test_data);

    assert_eq!(parser.parse(&test_data).unwrap(), expected1);
    assert_eq!(parser.parse(&test_data).unwrap(), expected2);
    assert_eq!(parser.parse(&test_data).unwrap(), expected3);
    assert_eq!(parser.parse(&test_data).unwrap(), expected4);
}

#[test]
fn test_empty() {
    let test_data = "".to_string().into_bytes();

    let mut parser = PsycListParser::new();

    //parser.set_buffer(&test_data);

    assert_eq!(parser.parse(&test_data).unwrap(), PsycListParserResult::Complete);
}

#[test]
fn test_empty_element() {
    let test_data = "|".to_string().into_bytes();

    let mut parser = PsycListParser::new();

    let expected = PsycListParserResult::ListElement {
        value: b""
    };

    assert_eq!(parser.parse(&test_data).unwrap(), expected);
}

#[test]
fn test_incomplete() {
    let test_data1 = "|8 element".to_string().into_bytes();
    let test_data2 = "1| element2|".to_string().into_bytes();

    let expected = vec![PsycListParserResult::ListElementStart {
                            value_part: b"element"
                        },
                        PsycListParserResult::ListElementEnd {
                            value_part: b"1"
                        },
                        PsycListParserResult::ListElement {
                            value: b"element2"
                        }];

    let mut parser = PsycListParser::new();

    //parser.set_buffer(&test_data1);
    assert_eq!(parser.parse(&test_data1).unwrap(), expected[0]);

    //parser.set_buffer(&test_data2);
    assert_eq!(parser.parse(&test_data2).unwrap(), expected[1]);

    assert_eq!(parser.parse(&test_data2).unwrap(), expected[2]);
}

#[test]
fn test_insufficient() {
    let mut test_data1 = "|4".to_string().into_bytes();
    let mut test_data2 = " foo1".to_string().into_bytes();

    let expected = vec![PsycListParserResult::InsufficientData,
                        PsycListParserResult::ListElement {
                            value: b"foo1"
                        }];

    let mut parser = PsycListParser::new();

    //parser.set_buffer(&test_data1);

    assert_eq!(parser.parse(&test_data1).unwrap(), expected[0]);

    let unparsed_length = parser.copy_unparsed_into_buffer(&mut test_data1);
    test_data1.resize(unparsed_length, 0);
    test_data1.append(&mut test_data2);
    //parser.set_buffer(&test_data1);
    assert_eq!(parser.parse(&test_data1).unwrap(), expected[1]);
}
