module psyc.syntax;

const PSYC_LIST_SIZE_LIMIT = 404;

/* beyond this a content length must be provided */
const PSYC_CONTENT_SIZE_THRESHOLD = 444;

/* beyond this a modifier value length must be provided */
const PSYC_MODIFIER_SIZE_THRESHOLD = 404;

const C_GLYPH_PACKET_DELIMITER = '|';
const S_GLYPH_PACKET_DELIMITER = "|";
const PSYC_PACKET_DELIMITER = "\n|\n";

const C_GLYPH_SEPARATOR_KEYWORD = '_';
const S_GLYPH_SEPARATOR_KEYWORD = "_";

const C_GLYPH_OPERATOR_SET = ':';
const S_GLYPH_OPERATOR_SET = ":";

const C_GLYPH_OPERATOR_ASSIGN = '=';
const S_GLYPH_OPERATOR_ASSIGN = "=";

const C_GLYPH_OPERATOR_AUGMENT = '+';
const S_GLYPH_OPERATOR_AUGMENT = "+";

const C_GLYPH_OPERATOR_DIMINISH = '-';
const S_GLYPH_OPERATOR_DIMINISH = "-";

const C_GLYPH_OPERATOR_QUERY = '?';
const S_GLYPH_OPERATOR_QUERY = "?";

/* might move into routing.h or something */
const PSYC_ROUTING = 1;
const PSYC_ROUTING_MERGE = 2;
const PSYC_ROUTING_RENDER = 4;
