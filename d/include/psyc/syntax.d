module psyc.syntax;

const PSYC_LIST_SIZE_LIMIT = 404;

/* beyond this a content length must be provided */
const PSYC_CONTENT_SIZE_THRESHOLD = 444;

/* beyond this a modifier value length must be provided */
const PSYC_MODIFIER_SIZE_THRESHOLD = 404;

const PSYC_PACKET_DELIMITER_CHAR = '|';
const PSYC_PACKET_DELIMITER =    "\n|\n";

/* might move into routing.h or something */
const PSYC_ROUTING = 1;
const PSYC_ROUTING_MERGE = 2;
const PSYC_ROUTING_RENDER = 4;
