extern crate psyc;
use psyc::parser::*;

#[test]
fn test_parse() {
    let test_data = ":_target\tpsyc://ve.symlynx.com/@blog\n\n?\n|\n".to_string().into_bytes();

    let expected = vec![PsycParserResult::RoutingModifier{
                            operator: ':', 
                            name: &test_data[1 .. 8],
                            value: &test_data[9 .. 36],
                        },
                        PsycParserResult::StateSync];

    let mut parser = PsycParser::new();

    parser.set_buffer(&test_data);

    assert_eq!(parser.parse().unwrap(), expected[0]);

    assert_eq!(parser.parse().unwrap(), expected[1]);
}

#[test]
fn test_insufficient() {
    let test_data = ":_target\tpsyc://ve.symlynx.com/@blog\n\n:_nick\tlurchi\n|\n".to_string().into_bytes();

    let expected = vec![PsycParserResult::InsufficientData,
                                PsycParserResult::RoutingModifier {
                                    operator: ':',
                                    name: &test_data[1 .. 8],
                                    value: &test_data[9 .. 36]
                                },
                                PsycParserResult::InsufficientData,
                                PsycParserResult::EntityModifier{
                                    operator: ':', 
                                    name: &test_data[39 .. 44],
                                    value: &test_data[45 .. 51],
                                },
                                PsycParserResult::Complete];
    
    let mut parser = PsycParser::new();

    parser.set_buffer(&test_data[.. 1]);
    assert_eq!(parser.parse().unwrap(), expected[0]);
    
    parser.set_buffer(&test_data[.. 46]);
    assert_eq!(parser.parse().unwrap(), expected[1]);
    
    parser.set_buffer(&test_data[.. 49]);
    assert_eq!(parser.parse().unwrap(), expected[2]);
    
    parser.set_buffer(&test_data);
    assert_eq!(parser.parse().unwrap(), expected[3]);
    
    assert_eq!(parser.parse().unwrap(), expected[4]);
}
