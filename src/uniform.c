#include <ctype.h>
#include "lib.h"
#include "psyc/uniform.h"
#include "psyc/parse.h"

/**
 * Parse uniform.
 *
 * @return PsycScheme on success, PsycParseUniformRC on error.
 */
int
psyc_uniform_parse (PsycUniform *uni, const char *buffer, size_t length)
{
    char c;
    PsycString *p;
    char *data = (char*)buffer;
    size_t pos = 0, part = PSYC_UNIFORM_SCHEME;

    uni->valid = 0;
    uni->full = PSYC_STRING(data, length);

    while (pos < length) {
	c = data[pos];
	if (c == ':') {
	    uni->scheme = PSYC_STRING(data, pos++);
	    break;
	} else if (!psyc_is_host_char(c))
	    return PSYC_PARSE_UNIFORM_INVALID_SCHEME;
	pos++;
    }

    p = &uni->scheme;
    if (p->length == 4 && (tolower(p->data[0]) == 'p' &&
			   tolower(p->data[1]) == 's' &&
			   tolower(p->data[2]) == 'y' &&
			   tolower(p->data[3]) == 'c')) {
	uni->type = PSYC_SCHEME_PSYC;
	part = PSYC_UNIFORM_SLASHES;
	uni->slashes = PSYC_STRING(data + pos, 0);

	while (pos < length) {
	    c = data[pos];
	    switch (part) {
	    case PSYC_UNIFORM_SLASHES:
		if (c == '/')
		    uni->slashes.length++;
		else
		    return PSYC_PARSE_UNIFORM_INVALID_SLASHES;

		if (uni->slashes.length == 2) {
		    part = PSYC_UNIFORM_HOST;
		    uni->host = PSYC_STRING(data + pos + 1, 0);
		}
		break;

	    case PSYC_UNIFORM_HOST:
		if (psyc_is_host_char(c)) {
		    uni->host.length++;
		    break;
		}

		if (uni->host.length == 0)
		    return PSYC_PARSE_UNIFORM_INVALID_HOST;

		if (c == ':') {
		    part = PSYC_UNIFORM_PORT;
		    p = &uni->port;
		} else if (c == '/') {
		    uni->slash = PSYC_STRING(data + pos, 1);

		    part = PSYC_UNIFORM_RESOURCE;
		    p = &uni->resource;
		} else
		    return PSYC_PARSE_UNIFORM_INVALID_HOST;

		*p = PSYC_STRING(data + pos + 1, 0);
		break;

	    case PSYC_UNIFORM_PORT:
		if (psyc_is_numeric(c) || (uni->port.length == 0 && c == '-')) {
		    uni->port.length++;
		    break;
		}

		if (uni->port.length == 0 && c != PSYC_TRANSPORT_GNUNET)
		    return PSYC_PARSE_UNIFORM_INVALID_PORT;

		if (c == '/') {
		    uni->slash = PSYC_STRING(data + pos, 1);

		    part = PSYC_UNIFORM_RESOURCE;
		    uni->resource = PSYC_STRING(data + pos + 1, 0);
		    break;
		} else {
		    part = PSYC_UNIFORM_TRANSPORT;
		    uni->transport = PSYC_STRING(data + pos, 0);
		}
		// fall thru

	    case PSYC_UNIFORM_TRANSPORT:
		switch (c) {
		case PSYC_TRANSPORT_GNUNET:
		    if (uni->port.length > 0)
			return PSYC_PARSE_UNIFORM_INVALID_TRANSPORT;
		case PSYC_TRANSPORT_TCP:
		case PSYC_TRANSPORT_UDP:
		case PSYC_TRANSPORT_TLS:
		    if (uni->transport.length > 0)
			return PSYC_PARSE_UNIFORM_INVALID_TRANSPORT;
		    uni->transport.length++;
		    break;
		case '/':
		    uni->slash = PSYC_STRING(data + pos, 1);

		    part = PSYC_UNIFORM_RESOURCE;
		    uni->resource = PSYC_STRING(data + pos + 1, 0);
		    break;
		default:
		    return PSYC_PARSE_UNIFORM_INVALID_TRANSPORT;
		}
		break;

	    case PSYC_UNIFORM_RESOURCE:
		if (psyc_is_name_char(c)) {
		    uni->resource.length++;
		    break;
		} else if (c == '#') {
		    part = PSYC_UNIFORM_CHANNEL;
		    uni->channel = PSYC_STRING(data + pos + 1, 0);
		    break;
		} else
		    return PSYC_PARSE_UNIFORM_INVALID_RESOURCE;

	    case PSYC_UNIFORM_CHANNEL:
		if (psyc_is_name_char(c)) {
		    uni->channel.length++;
		    break;
		} else
		    return PSYC_PARSE_UNIFORM_INVALID_CHANNEL;
	    }
	    pos++;
	}

	if (uni->host.length == 0)
	    return PSYC_PARSE_UNIFORM_INVALID_HOST;

	uni->host_port = PSYC_STRING(uni->host.data, uni->host.length
				     + uni->port.length + uni->transport.length);
	if (uni->port.length > 0 || uni->transport.length > 0)
	    uni->host_port.length++;

	uni->root = PSYC_STRING(data, uni->scheme.length + 1
				+ uni->slashes.length + uni->host_port.length);

	uni->entity = PSYC_STRING(data, uni->root.length + uni->slash.length
				  + uni->resource.length);

	uni->body = PSYC_STRING(uni->host.data,
				length - uni->scheme.length - 1 - uni->slashes.length);

	if (uni->slash.length) {
	    uni->path.data = uni->slash.data;
	    uni->path.length = length - uni->root.length;
	}

	if (uni->resource.length)
	    uni->nick = PSYC_STRING(uni->resource.data + 1, uni->resource.length);

    } else
	return PSYC_PARSE_UNIFORM_INVALID_SCHEME;

    if (uni->host.length == 0)
	return PSYC_PARSE_UNIFORM_INVALID_HOST;

    uni->valid = 1;
    return uni->type;
}
