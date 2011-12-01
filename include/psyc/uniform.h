#ifndef PSYC_UNIFORM_H
#define PSYC_UNIFORM_H

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
} PsycScheme;

typedef struct {
// essential parts
    uint8_t valid;
    PsycScheme type;
    PsycString scheme;
    PsycString user;
    PsycString pass;
    PsycString host;
    PsycString port;
    PsycString transport;
    PsycString resource;
    PsycString query;
    PsycString channel;

// convenient snippets of the URL
    PsycString full;		// the URL as such
    PsycString body;		// the URL without scheme and '//'
    PsycString user_host;	// mailto and xmpp style
    PsycString host_port;	// just host:port (and transport)
    PsycString root;		// root UNI
    PsycString entity;		// entity UNI, without the channel
    PsycString slashes;		// the // if the protocol has them
    PsycString slash;		// first / after host
    PsycString nick;		// whatever works as a nickname
} PsycUniform;

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
} PsycUniformPart;

typedef enum {
    PSYC_PARSE_UNIFORM_INVALID_SLASHES = -7,
    PSYC_PARSE_UNIFORM_INVALID_CHANNEL = -6,
    PSYC_PARSE_UNIFORM_INVALID_RESOURCE = -5,
    PSYC_PARSE_UNIFORM_INVALID_TRANSPORT = -4,
    PSYC_PARSE_UNIFORM_INVALID_PORT = -3,
    PSYC_PARSE_UNIFORM_INVALID_HOST = -2,
    PSYC_PARSE_UNIFORM_INVALID_SCHEME = -1,
} PsycParseUniformRC;

typedef enum {
    PSYC_TRANSPORT_TCP = 'c',
    PSYC_TRANSPORT_UDP = 'd',
    PSYC_TRANSPORT_TLS = 's',
    PSYC_TRANSPORT_GNUNET = 'g',
} PsycTransport;

typedef enum {
    PSYC_ENTITY_ROOT = 0,
    PSYC_ENTITY_PERSON = '~',
    PSYC_ENTITY_PLACE = '@',
    PSYC_ENTITY_SERVICE = '$',
} PsycEntityType;

int
psyc_uniform_parse (PsycUniform *uni, const char *buffer, size_t length);

#endif
