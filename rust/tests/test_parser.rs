extern crate psyc;
use psyc::parser::*;

#[test]
fn test_parse() {
    let test_data = ":_target\tpsyc://ve.symlynx.com/@blog\n\n?\n|\n".to_string().into_bytes();

    let expected1 = 
    PsycParserResult::RoutingModifier{
        operator: ':', 
        name: &test_data[1 .. 8],
        value: &test_data[9 .. 36],
    };

    let expected2 = PsycParserResult::StateSync;

    let mut parser = PsycParser::new();

    parser.set_buffer(&test_data);

    {
        let result1 = parser.parse();
        assert_eq!(result1, Ok(expected1));
    }

    {
        let result2 = parser.parse();
        assert_eq!(result2, Ok(expected2));
    }
}

#[test]
fn test_insufficient() {
    let test_data = ":_target\tpsyc://ve.symlynx.com/@blog\n\n:_nick\tlurchi\n|\n".to_string().into_bytes();

    let expected1 = PsycParserResult::InsufficientData;
    let expected2 =PsycParserResult::RoutingModifier {
        operator: ':',
        name: &test_data[1 .. 8],
        value: &test_data[9 .. 36]
    };
    let expected3 = PsycParserResult::InsufficientData;
    let expected4 = PsycParserResult::EntityModifier{
        operator: ':', 
        name: &test_data[39 .. 44],
        value: &test_data[45 .. 51],
    };
    let expected5 = PsycParserResult::Complete;
    
    let mut parser = PsycParser::new();
    
    {
        parser.set_buffer(&test_data[.. 1]);
        let result1 = parser.parse();
        assert_eq!(result1, Ok(expected1));
    }
     
    {
        parser.set_buffer(&test_data[.. 46]);
        let result2 = parser.parse();
        assert_eq!(result2, Ok(expected2));
    }

    {
        parser.set_buffer(&test_data[.. 49]);
        let result3 = parser.parse();
        assert_eq!(result3, Ok(expected3));
    }

    {
        parser.set_buffer(&test_data);
        let result4 = parser.parse();
        assert_eq!(result4, Ok(expected4));
    }

    {
        let result5 = parser.parse();
        assert_eq!(result5, Ok(expected5));
    }
}
