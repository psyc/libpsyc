extern crate psyc;

use psyc::parser::*;

#[test]
fn test_parse() {
    let test_data = ":_target\tpsyc://ve.symlynx.com/@blog\n\n?\n|\n".to_string().into_bytes();

    let expected1 = 
    PsycParserResult::RoutingModifier{
        operator: ':', 
        name: "_target".to_string(),
        value: "psyc://ve.symlynx.com/@blog".to_string().into_bytes()
    };

    let expected2 = PsycParserResult::StateSync;

    let mut parser = PsycParser::new();
    parser.set_buffer(&test_data);

    let result1 = parser.parse();
    let result2 = parser.parse();

    assert_eq!(result1, Ok(expected1));
    assert_eq!(result2, Ok(expected2));
}
