/*
  This file is part of libpsyc.
  Copyright (C) 2011,2012 Carlo v. Loesch, Gabor X Toth, Mathias L. Baumann,
  and other contributing authors.

  libpsyc is free software: you can redistribute it and/or modify it under the
  terms of the GNU Affero General Public License as published by the Free
  Software Foundation, either version 3 of the License, or (at your option) any
  later version. As a special exception, libpsyc is distributed with additional
  permissions to link libpsyc libraries with non-AGPL works.

  libpsyc is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
  details.

  You should have received a copy of the GNU Affero General Public License and
  the linking exception along with libpsyc in a COPYING file.
*/

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
    PsycString path;		// path begins at first / after host
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

/**
 * Accepted transport flags in PSYC uniforms.
 * Appears after the hostname, the colon and the optional port number
 */
typedef enum {
    PSYC_TRANSPORT_TCP = 'c',
    PSYC_TRANSPORT_UDP = 'd',
    PSYC_TRANSPORT_TLS = 's',
    PSYC_TRANSPORT_GNUNET = 'g',
} PsycTransport;

/** Characters indicating typical kinds of PSYC entities. */
typedef enum {
    PSYC_ENTITY_ROOT = 0,
    PSYC_ENTITY_PERSON = '~',
    PSYC_ENTITY_PLACE = '@',
    PSYC_ENTITY_SERVICE = '$',
} PsycEntityType;

/**
 * Parse uniform.
 *
 * @return PsycScheme on success, PsycParseUniformRC on error.
 */
int
psyc_uniform_parse (PsycUniform *uni, const char *buffer, size_t length);

/**
 * Get entity type.
 *
 * @return PsycEntityType on success, PSYC_ERROR on error.
 */
static inline int
psyc_entity_type (char entity)
{
    switch (entity) {
    case PSYC_ENTITY_PERSON:
	return PSYC_ENTITY_PERSON;
    case PSYC_ENTITY_PLACE:
	return PSYC_ENTITY_PLACE;
    case PSYC_ENTITY_SERVICE:
	return PSYC_ENTITY_SERVICE;
    default:
	return PSYC_ERROR;
    }
}

#endif
