#ifndef PSYC_UNIFORM_H
/**
 * @file uniform.h
 * @brief Uniform parsing.
 */

#include <psyc.h>

typedef enum {
	PSYC_SCHEME_PSYC = 0,
	PSYC_SCHEME_IRC = 1,
	PSYC_SCHEME_XMPP = 2,
	PSYC_SCHEME_SIP = 3,
} psycScheme;

typedef struct {
// essential parts
	uint8_t valid;
	psycScheme type;
	psycString scheme;
	psycString user;
	psycString pass;
	psycString host;
	psycString port;
	psycString transport;
	psycString resource;
	psycString query;
	psycString channel;

// convenient snippets of the URL
	psycString full;       // the URL as such
	psycString body;       // the URL without scheme and '//'
	psycString user_host;  // mailto and xmpp style
	psycString host_port;  // just host:port (and transport)
	psycString root;       // root UNI of peer/server
	psycString slashes;    // the // if the protocol has them
	psycString nick;       // whatever works as a nickname
} psycUniform;

typedef enum {
	PSYC_UNIFORM_SCHEME = 0,
	PSYC_UNIFORM_SLASHES,
	PSYC_UNIFORM_USER,
	PSYC_UNIFORM_PASS,
	PSYC_UNIFORM_HOST,
	PSYC_UNIFORM_PORT,
	PSYC_UNIFORM_TRANSPORT,
	PSYC_UNIFORM_RESOURCE,
	PSYC_UNIFORM_QUERY,
	PSYC_UNIFORM_CHANNEL,
} psycUniformPart;

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

int psyc_uniform_parse (psycUniform *uni, char *str, size_t length);

#define PSYC_UNIFORM_H
#endif
