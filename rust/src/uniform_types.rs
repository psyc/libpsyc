use types::PsycString;

#[derive(Debug, PartialEq)]
pub enum PsycEntity<'a> {
    Root,
    Person {
        name: &'a str,
        channel: &'a str
    },
    Place {
        name: &'a str,
        channel: &'a str
    },
    Service {
        name: &'a str,
        channel: &'a str
    },
    Unknown {
        object: &'a str,
        channel: &'a str
    }
}

#[derive(Clone, Copy)]
#[derive(Debug, PartialEq)]
#[repr(C)]
pub enum PsycScheme {
    PSYC_SCHEME_PSYC = 0,
    PSYC_SCHEME_IRC = 1,
    PSYC_SCHEME_XMPP = 2,
    PSYC_SCHEME_SIP = 3
}

#[repr(C)]
pub enum PsycEntityType {
    PSYC_ENTITY_ROOT = 0,
    PSYC_ENTITY_PERSON = '~' as _,
    PSYC_ENTITY_PLACE = '@' as _,
    PSYC_ENTITY_SERVICE = '$' as _
}

#[derive(Debug, PartialEq)]
#[repr(C)]
pub enum PsycTransport {
    PSYC_TRANSPORT_TCP = 'c' as _,
    PSYC_TRANSPORT_UDP = 'd' as _,
    PSYC_TRANSPORT_TLS = 's' as _,
    PSYC_TRANSPORT_GNUNET = 'g' as _,
}

#[derive(Debug, PartialEq)]
#[repr(C)]
pub enum UniformParseError {
    PSYC_PARSE_UNIFORM_INVALID_SLASHES = -7,
    PSYC_PARSE_UNIFORM_INVALID_CHANNEL = -6,
    PSYC_PARSE_UNIFORM_INVALID_RESOURCE = -5,
    PSYC_PARSE_UNIFORM_INVALID_TRANSPORT = -4,
    PSYC_PARSE_UNIFORM_INVALID_PORT = -3,
    PSYC_PARSE_UNIFORM_INVALID_HOST = -2,
    PSYC_PARSE_UNIFORM_INVALID_SCHEME = -1,
}

#[derive(Debug)]
#[repr(C)]
pub struct PsycUniform {
    pub valid: bool,
    pub uniform_type: PsycScheme,
    pub scheme: PsycString,
    pub user: PsycString,
    pub pass: PsycString,
    pub host: PsycString,
    pub port: PsycString,
    pub transport: PsycString,
    pub resource: PsycString,
    pub query: PsycString,
    pub channel: PsycString,

    pub full: PsycString,
    pub body: PsycString,
    pub user_host: PsycString,
    pub host_port: PsycString,
    pub root: PsycString,
    pub entity: PsycString,
    pub slashes: PsycString,
    pub slash: PsycString,
    pub path: PsycString,
    pub nick: PsycString
}
