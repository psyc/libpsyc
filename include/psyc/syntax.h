#ifndef PSYC_SYNTAX_H

#ifndef PSYC_LIST_SIZE_LIMIT
# define PSYC_LIST_SIZE_LIMIT 404
#endif

/* beyond this a content length must be provided */
#ifndef PSYC_CONTENT_SIZE_THRESHOLD
# define PSYC_CONTENT_SIZE_THRESHOLD 444
#endif

/* beyond this a modifier value length must be provided */
#ifndef PSYC_MODIFIER_SIZE_THRESHOLD
# define PSYC_MODIFIER_SIZE_THRESHOLD 404
#endif

#define PSYC_PACKET_DELIMITER_CHAR '|'
#define PSYC_PACKET_DELIMITER	   "\n|\n"

#define PSYC_SYNTAX_H
#endif
