#include <ctype.h>
#include "lib.h"
#include "psyc/uniform.h"
#include "psyc/parse.h"

int psyc_parseUniform2 (psycUniform *uni, const char *str, size_t length)
{
	char c;
	psycString *p;
	size_t pos = 0, part = PSYC_UNIFORM_SCHEME;

	uni->valid = 0;
	uni->full.ptr = str;
	uni->full.length = length;

	while (pos < length) {
		c = str[pos];
		if (c == ':') {
			uni->scheme.ptr = str;
			uni->scheme.length = pos++;
			break;
		} else if (!psyc_isHostChar(c))
			return PSYC_PARSE_UNIFORM_INVALID_SCHEME;
		pos++;
	}

	p = &uni->scheme;
	if (p->length == 4 &&
	    tolower(p->ptr[0]) == 'p' &&
	    tolower(p->ptr[1]) == 's' &&
	    tolower(p->ptr[2]) == 'y' &&
	    tolower(p->ptr[3]) == 'c') {

		uni->type = PSYC_SCHEME_PSYC;
		part = PSYC_UNIFORM_SLASHES;
		uni->slashes.ptr = str + pos;
		uni->slashes.length = 0;

		while (pos < length) {
			c = str[pos];
			switch (part) {
				case PSYC_UNIFORM_SLASHES:
					if (c == '/')
						uni->slashes.length++;
					else return PSYC_PARSE_UNIFORM_INVALID_SLASHES;

					if (uni->slashes.length == 2) {
						part = PSYC_UNIFORM_HOST;
						uni->host.ptr = str + pos + 1;
						uni->host.length = 0;
					}
					break;

				case PSYC_UNIFORM_HOST:
					if (psyc_isHostChar(c)) {
						uni->host.length++;
						break;
					}

					if (uni->host.length == 0)
						return PSYC_PARSE_UNIFORM_INVALID_HOST;

					if (c == ':') {
						part = PSYC_UNIFORM_PORT;
						p = &uni->port;
					} else if (c == '/') {
						part = PSYC_UNIFORM_RESOURCE;
						p = &uni->resource;
					}
					else return PSYC_PARSE_UNIFORM_INVALID_HOST;

					p->ptr = str + pos + 1;
					p->length = 0;
					break;

				case PSYC_UNIFORM_PORT:
					if (psyc_isNumeric(c)) {
						uni->port.length++;
						break;
					}

					if (uni->port.length == 0 && c != PSYC_TRANSPORT_GNUNET)
						return PSYC_PARSE_UNIFORM_INVALID_PORT;

					if (c == '/') {
						part = PSYC_UNIFORM_RESOURCE;
						uni->resource.ptr = str + pos + 1;
						uni->resource.length = 0;
						break;
					}
					else {
						part = PSYC_UNIFORM_TRANSPORT;
						uni->transport.ptr = str + pos;
						uni->transport.length = 0;
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
							part = PSYC_UNIFORM_RESOURCE;
							uni->resource.ptr = str + pos + 1;
							uni->resource.length = 0;
							break;
						default:
							return PSYC_PARSE_UNIFORM_INVALID_TRANSPORT;
					}
					break;

				case PSYC_UNIFORM_RESOURCE:
					if (psyc_isNameChar(c)) {
						uni->resource.length++;
						break;
					} else if (c == '#') {
						part = PSYC_UNIFORM_CHANNEL;
						uni->channel.ptr = str + pos + 1;
						uni->channel.length = 0;
						break;
					} else return PSYC_PARSE_UNIFORM_INVALID_RESOURCE;

				case PSYC_UNIFORM_CHANNEL:
					if (psyc_isNameChar(c)) {
						uni->channel.length++;
						break;
					} else return PSYC_PARSE_UNIFORM_INVALID_CHANNEL;
			}
			pos++;
		}

		if (uni->host.length == 0)
			return PSYC_PARSE_UNIFORM_INVALID_HOST;

		uni->host_port.ptr = uni->host.ptr;
		uni->host_port.length = uni->host.length + uni->port.length + uni->transport.length;
		if (uni->port.length > 0 || uni->transport.length > 0)
			uni->host_port.length++;

		uni->root.ptr = str;
		uni->root.length = uni->scheme.length + 1 + uni->slashes.length +
			uni->host_port.length;

		uni->body.ptr = uni->host.ptr;
		uni->body.length = length - uni->scheme.length - 1 - uni->slashes.length;

		if (uni->resource.length) {
			uni->nick.ptr = uni->resource.ptr + 1;
			uni->nick.length = uni->resource.length;
		}

	} else return PSYC_PARSE_UNIFORM_INVALID_SCHEME;

  if (uni->host.length == 0)
		return PSYC_PARSE_UNIFORM_INVALID_HOST;

	uni->valid = 1;
	return uni->type;
}

int psyc_parseUniform (psycUniform *uni, psycString *str)
{
	return psyc_parseUniform2(uni, str->ptr, str->length);
}
