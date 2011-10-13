#ifndef PSYC_UNIFORM_H
/**
 * @file uniform.h
 * @brief Uniform parsing.
 */

#include <psyc.h>

typedef enum {
// essential parts
	PSYC_UNIFORM_SCHEME = 0,
	PSYC_UNIFORM_USER = 1,
	PSYC_UNIFORM_PASS = 2,
	PSYC_UNIFORM_HOST = 3,
	PSYC_UNIFORM_PORT = 4,
	PSYC_UNIFORM_TRANSPORT = 5,
	PSYC_UNIFORM_RESOURCE = 6,
	PSYC_UNIFORM_QUERY = 7,
	PSYC_UNIFORM_CHANNEL = 8,

// convenient snippets of the URL
	PSYC_UNIFORM_FULL = 9,        // the URL as such
	PSYC_UNIFORM_BODY = 10,       // the URL without scheme and '//'
	PSYC_UNIFORM_USERATHOST = 11,	// mailto and xmpp style
	PSYC_UNIFORM_HOSTPORT = 12,   // just host:port (and transport)
	PSYC_UNIFORM_ROOT = 13,       // root UNI of peer/server
	PSYC_UNIFORM_SLASHES = 14,    // the // if the protocol has them
	PSYC_UNIFORM_NICK = 15,       // whatever works as a nickname
	PSYC_UNIFORM_SIZE = 16,
} psycUniformField;

typedef enum {
	PSYC_PARSE_UNIFORM_INVALID_SLASHES = -7,
	PSYC_PARSE_UNIFORM_INVALID_CHANNEL = -6,
	PSYC_PARSE_UNIFORM_INVALID_RESOURCE = -5,
	PSYC_PARSE_UNIFORM_INVALID_TRANSPORT = -4,
	PSYC_PARSE_UNIFORM_INVALID_PORT = -3,
	PSYC_PARSE_UNIFORM_INVALID_HOST = -2,
	PSYC_PARSE_UNIFORM_INVALID_SCHEME = -1,
} psycParseUniformRC;

typedef enum {
	PSYC_SCHEME_PSYC = 0,
	PSYC_SCHEME_IRC = 1,
	PSYC_SCHEME_XMPP = 2,
	PSYC_SCHEME_SIP = 3,
} psycScheme;

typedef enum {
	PSYC_TRANSPORT_TCP = 'c',
	PSYC_TRANSPORT_UDP = 'd',
	PSYC_TRANSPORT_TLS = 's',
	PSYC_TRANSPORT_GNUNET = 'g',
} psycTransport;

typedef enum {
	PSYC_ENTITY_PERSON = '~',
	PSYC_ENTITY_PLACE = '@',
	PSYC_ENTITY_SERVICE = '$',
} psycEntityType;

typedef psycString psycUniform[PSYC_UNIFORM_SIZE];

int psyc_parseUniform2(psycUniform *uni, const char *str, size_t length);

int psyc_parseUniform(psycUniform *uni, psycString *str);

#define PSYC_UNIFORM_H
#endif
