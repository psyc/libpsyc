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

#define C_GLYPH_PACKET_DELIMITER	'|'
#define S_GLYPH_PACKET_DELIMITER	"|"
#define PSYC_PACKET_DELIMITER	      "\n|\n"

#define	C_GLYPH_SEPARATOR_KEYWORD	'_'
#define	S_GLYPH_SEPARATOR_KEYWORD	"_"

#define	C_GLYPH_OPERATOR_SET		':'
#define	S_GLYPH_OPERATOR_SET		":"

#define	C_GLYPH_OPERATOR_ASSIGN		'='
#define	S_GLYPH_OPERATOR_ASSIGN		"="

#define	C_GLYPH_OPERATOR_AUGMENT	'+'
#define	S_GLYPH_OPERATOR_AUGMENT	"+"

#define	C_GLYPH_OPERATOR_DIMINISH	'-'
#define	S_GLYPH_OPERATOR_DIMINISH	"-"

#define	C_GLYPH_OPERATOR_QUERY		'?'
#define	S_GLYPH_OPERATOR_QUERY		"?"

/* might move into routing.h or something */
#define PSYC_ROUTING			1
#define PSYC_ROUTING_MERGE		2
#define PSYC_ROUTING_RENDER		4

#define PSYC_SYNTAX_H
#endif
