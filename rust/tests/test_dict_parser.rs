extern crate psyc;
use psyc::parser::*;

//#[test]
//fn test_parse() {
//    let test_data = "{foo1} bar1{foo2} bar2".to_string().into_bytes();
//
//    let expected1 = PsycDictParserResult::DictEntry{
//        key: b"foo1",
//        value: b"bar1"
//    };
//
//    let expected2 = PsycDictParserResult::DictEntry{
//        key: b"foo2",
//        value: b"bar2"
//    };
//
//    let expected3 = PsycDictParserResult::Complete;
//
//    let mut parser = PsycDictParser::new();
//
//    parser.set_buffer(&test_data);
//
//    assert_eq!(parser.parse().unwrap(), expected1);
//    assert_eq!(parser.parse().unwrap(), expected2);
//    assert_eq!(parser.parse().unwrap(), expected3);
//}
//
//#[test]
//fn test_empty() {
//    let test_data = "".to_string().into_bytes();
//
//    let mut parser = PsycDictParser::new();
//
//    parser.set_buffer(&test_data);
//
//    assert_eq!(parser.parse().unwrap(), PsycDictParserResult::Complete);
//}
//
//#[test]
//fn test_incomplete() {
//    let test_data1 = "{foo1}4 bar".to_string().into_bytes();
//    let test_data2 = "1{foo2} bar2".to_string().into_bytes();
//
//    let expected = vec![PsycDictParserResult::DictEntryStart {
//                            key: b"foo1",
//                            value_part: b"bar"    
//                        },
//                        PsycDictParserResult::DictEntryEnd {
//                           value_part: b"1"
//                        },
//                        PsycDictParserResult::DictEntry {
//                           key: b"foo2",
//                           value: b"bar2"
//                        }];
//
//    let mut parser = PsycDictParser::new();
//
//    parser.set_buffer(&test_data1);
//    assert_eq!(parser.parse().unwrap(), expected[0]);
//
//    parser.set_buffer(&test_data2);
//    assert_eq!(parser.parse().unwrap(), expected[1]);
//
//    assert_eq!(parser.parse().unwrap(), expected[2]);
//}
//
////#[test]
////fn test_insufficient() {
////    let mut test_data1 = "{4 foo".to_string().into_bytes();
////    let mut test_data2 = "1} bar1".to_string().into_bytes();
////    let mut test_data3 = "{4 foo1} bar1".to_string().into_bytes();
////
////    let expected = vec![PsycDictParserResult::InsufficientData,
////                        PsycDictParserResult::DictEntry {
////                            key: b"foo1",
////                            value: b"bar1"
////                        }];
////
////    let mut parser = PsycDictParser::new();
////
////    parser.set_buffer(&test_data1);
////    assert_eq!(parser.parse().unwrap(), expected[0]);
////
////    //let unparsed_length = parser.copy_unparsed_into_buffer(&mut test_data1);
////    //test_data1.resize(unparsed_length, 0);
////    //test_data1.append(&mut test_data2);
////    parser.set_buffer(&test_data3);
////    assert_eq!(parser.parse().unwrap(), expected[1]);
////}
