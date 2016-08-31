extern crate psyc;
use psyc::parser::*;

#[test]
fn test_parse() {
    let test_data = ":_target\tpsyc://ve.symlynx.com/@blog\n\n?\n|\n".to_string().into_bytes();

    let expected = vec![PsycParserResult::RoutingModifier{
                            operator: ':', 
                            name: "_target".as_bytes(),
                            value: "psyc://ve.symlynx.com/@blog".as_bytes(),
                        },
                        PsycParserResult::StateSync];

    let mut parser = PsycParser::new();

    parser.set_buffer(&test_data);

    assert_eq!(parser.parse().unwrap(), expected[0]);

    assert_eq!(parser.parse().unwrap(), expected[1]);
}

#[test]
fn test_insufficient() {
    let mut test_data1 = ":_target\tpsyc://ve.symlynx.com/@blog\n\n:_nick".to_string().into_bytes();
    let mut test_data2 = "\tlurchi\n|\n".to_string().into_bytes();

    let expected = vec![PsycParserResult::InsufficientData,
                        PsycParserResult::RoutingModifier {
                            operator: ':',
                            name: "_target".as_bytes(),
                            value: "psyc://ve.symlynx.com/@blog".as_bytes()
                        },
                        PsycParserResult::InsufficientData,
                        PsycParserResult::EntityModifier{
                            operator: ':', 
                            name: "_nick".as_bytes(),
                            value: "lurchi".as_bytes(),
                        },
                        PsycParserResult::Complete];
 
    let mut parser = PsycParser::new();

    parser.set_buffer(&test_data1[.. 1]);
    assert_eq!(parser.parse().unwrap(), expected[0]);
   
    let unparsed_length = parser.copy_unparsed_into_buffer(&mut test_data1);
    assert_eq!(unparsed_length, 1);
    parser.set_buffer(&test_data1[.. 44]);
    assert_eq!(parser.parse().unwrap(), expected[1]);
   
    assert_eq!(parser.parse().unwrap(), expected[2]);
    
    let unparsed_length = parser.copy_unparsed_into_buffer(&mut test_data1);
    test_data1.resize(unparsed_length, 0);
    test_data1.append(&mut test_data2);
    parser.set_buffer(&test_data1);
    assert_eq!(parser.parse().unwrap(), expected[3]);
    
    assert_eq!(parser.parse().unwrap(), expected[4]);
}
