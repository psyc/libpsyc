extern crate psyc;
use psyc::*;

#[test]
fn test_parse() {
    let test_data = ":_target\tpsyc://ve.symlynx.com/@blog\n\n?\n|\n".to_string().into_bytes();

    let expected = vec![PsycParserResult::RoutingModifier{
                            operator: PsycOperator::PSYC_OPERATOR_SET, 
                            name: "_target".as_bytes(),
                            value: "psyc://ve.symlynx.com/@blog".as_bytes(),
                        },
                        PsycParserResult::StateSync];

    let mut parser = PsycParser::new();

    assert_eq!(parser.parse(&test_data).unwrap(), expected[0]);
    assert_eq!(parser.parse(&test_data).unwrap(), expected[1]);
}

#[test]
fn test_empty() {
    let test_data = "".to_string().into_bytes();

    let mut parser = PsycParser::new();

    // FIXME: InsufficientData or Complete?
    assert_eq!(parser.parse(&test_data).unwrap(), PsycParserResult::InsufficientData);
}

#[test]
fn test_incomplete() {
    let test_data1 = ":_target\tpsyc://ve.symlynx.com/@blog\n\n:_nick 6\t".to_string().into_bytes();

    let test_data2 = "lurchi".to_string().into_bytes();

    let expected = vec![PsycParserResult::EntityModifierStart {
                            operator: PsycOperator::PSYC_OPERATOR_SET,
                            name: b"_nick",
                            value_part: b""
                        },
                        PsycParserResult::EntityModifierEnd {
                            value_part: b"lurchi"
                        }];

    let mut parser = PsycParser::new();

    let _ = parser.parse(&test_data1);
    assert_eq!(parser.parse(&test_data1).unwrap(), expected[0]);

    assert_eq!(parser.parse(&test_data2).unwrap(), expected[1]);
}

#[test]
fn test_insufficient() {
    let mut test_data1 = ":_target\tpsyc://ve.symlynx.com/@blog\n\n:_nick".to_string().into_bytes();
    let mut test_data2 = "\tlurchi\n|\n".to_string().into_bytes();

    let expected = vec![PsycParserResult::InsufficientData,
                        PsycParserResult::RoutingModifier {
                            operator: PsycOperator::PSYC_OPERATOR_SET,
                            name: b"_target",
                            value: b"psyc://ve.symlynx.com/@blog"
                        },
                        PsycParserResult::InsufficientData,
                        PsycParserResult::EntityModifier{
                            operator: PsycOperator::PSYC_OPERATOR_SET, 
                            name: b"_nick",
                            value: b"lurchi"
                        },
                        PsycParserResult::Complete];
 
    let mut parser = PsycParser::new();

    assert_eq!(parser.parse(&test_data1[.. 1]).unwrap(), expected[0]);
   
    let unparsed_length = parser.copy_unparsed_into_buffer(&mut test_data1);
    assert_eq!(unparsed_length, 1);
    assert_eq!(parser.parse(&test_data1[.. 44]).unwrap(), expected[1]);
    
    assert_eq!(parser.parse(&test_data1[.. 44]).unwrap(), expected[2]);
    
    let unparsed_length = parser.copy_unparsed_into_buffer(&mut test_data1);
    test_data1.resize(unparsed_length, 0);
    test_data1.append(&mut test_data2);
    assert_eq!(parser.parse(&test_data1).unwrap(), expected[3]);
    assert_eq!(parser.parse(&test_data1).unwrap(), expected[4]);
}

#[test]
fn test_multiple() {
    let test_data = b":_target\tpsyc://ve.symlynx.com/@blog\n\n?\n|\n:_target\tpsyc://ve.symlynx.com/@blog\n\n_test_method\n|\n";

    let expected = vec![PsycParserResult::RoutingModifier {
                            operator: PsycOperator::PSYC_OPERATOR_SET,
                            name: b"_target",
                            value: b"psyc://ve.symlynx.com/@blog"
                        },
                        PsycParserResult::StateSync,
                        PsycParserResult::Complete,
                        PsycParserResult::RoutingModifier {
                            operator: PsycOperator::PSYC_OPERATOR_SET,
                            name: b"_target",
                            value: b"psyc://ve.symlynx.com/@blog"
                        },
                        PsycParserResult::Body {
                            method: b"_test_method",
                            data: b""
                        },
                        PsycParserResult::Complete];

    let mut parser = PsycParser::new();

    for e in expected {
        assert_eq!(parser.parse(test_data).unwrap(), e);
    }
}
