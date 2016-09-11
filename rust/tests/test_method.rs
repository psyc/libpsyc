extern crate psyc;
use psyc::*;

#[test]
fn test_method_matching() {
    let m = Method {method: b"_notice_context_enter"};

    assert!(m.inherits(b"_notice_context"));
    assert!(m.matches(b"_context"));
}

#[test]
fn test_method_lookup() {
    let m = Method {method: b"_notice_context_enter"};

    let expected_flags = PSYC_METHOD_TEMPLATE |
                         PSYC_METHOD_VISIBLE |
                         PSYC_METHOD_LOGGABLE;

    assert_eq!(m.lookup(),
               MethodInfo {
                    lookup_result: PsycMethod::PSYC_MC_NOTICE_CONTEXT_ENTER,
                    family: PsycMethod::PSYC_MC_NOTICE,
                    flags: expected_flags
               });
}

#[test]
fn test_lookup_empty() {
    let m = Method {method: b""};

    assert_eq!(m.lookup(),
               MethodInfo {
                   lookup_result: PsycMethod::PSYC_MC_UNKNOWN,
                   family: PsycMethod::PSYC_MC_UNKNOWN,
                   flags: PsycMethodFlags::empty()
               });
}
