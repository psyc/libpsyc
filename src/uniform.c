#include <ctype.h>
#include "lib.h"
#include "psyc/uniform.h"
#include "psyc/parse.h"

int psyc_uniform_parse (PsycUniform *uni, char *str, size_t length)
{
	char c;
	PsycString *p;
	size_t pos = 0, part = PSYC_UNIFORM_SCHEME;

	uni->valid = 0;
	uni->full.data = str;
	uni->full.length = length;

	while (pos < length) {
		c = str[pos];
		if (c == ':') {
			uni->scheme.data = str;
			uni->scheme.length = pos++;
			break;
		} else if (!psyc_is_host_char(c))
			return PSYC_PARSE_UNIFORM_INVALID_SCHEME;
		pos++;
	}

	p = &uni->scheme;
	if (p->length == 4 &&
	    tolower(p->data[0]) == 'p' &&
	    tolower(p->data[1]) == 's' &&
	    tolower(p->data[2]) == 'y' &&
	    tolower(p->data[3]) == 'c') {

		uni->type = PSYC_SCHEME_PSYC;
		part = PSYC_UNIFORM_SLASHES;
		uni->slashes.data = str + pos;
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
						uni->host.data = str + pos + 1;
						uni->host.length = 0;
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
						uni->slash.data = str + pos;
						uni->slash.length = 1;

						part = PSYC_UNIFORM_RESOURCE;
						p = &uni->resource;
					}
					else return PSYC_PARSE_UNIFORM_INVALID_HOST;

					p->data = str + pos + 1;
					p->length = 0;
					break;

				case PSYC_UNIFORM_PORT:
					if (psyc_is_numeric(c) || (uni->port.length == 0 && c == '-')) {
						uni->port.length++;
						break;
					}

					if (uni->port.length == 0 && c != PSYC_TRANSPORT_GNUNET)
						return PSYC_PARSE_UNIFORM_INVALID_PORT;

					if (c == '/') {
						uni->slash.data = str + pos;
						uni->slash.length = 1;

						part = PSYC_UNIFORM_RESOURCE;
						uni->resource.data = str + pos + 1;
						uni->resource.length = 0;
						break;
					}
					else {
						part = PSYC_UNIFORM_TRANSPORT;
						uni->transport.data = str + pos;
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
							uni->slash.data = str + pos;
							uni->slash.length = 1;

							part = PSYC_UNIFORM_RESOURCE;
							uni->resource.data = str + pos + 1;
							uni->resource.length = 0;
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
						uni->channel.data = str + pos + 1;
						uni->channel.length = 0;
						break;
					} else return PSYC_PARSE_UNIFORM_INVALID_RESOURCE;

				case PSYC_UNIFORM_CHANNEL:
					if (psyc_is_name_char(c)) {
						uni->channel.length++;
						break;
					} else return PSYC_PARSE_UNIFORM_INVALID_CHANNEL;
			}
			pos++;
		}

		if (uni->host.length == 0)
			return PSYC_PARSE_UNIFORM_INVALID_HOST;

		uni->host_port.data = uni->host.data;
		uni->host_port.length = uni->host.length + uni->port.length + uni->transport.length;
		if (uni->port.length > 0 || uni->transport.length > 0)
			uni->host_port.length++;

		uni->root.data = str;
		uni->root.length = uni->scheme.length + 1 + uni->slashes.length +
			uni->host_port.length;

		uni->entity.data = str;
		uni->entity.length = uni->root.length + uni->slash.length + uni->resource.length;

		uni->body.data = uni->host.data;
		uni->body.length = length - uni->scheme.length - 1 - uni->slashes.length;

		if (uni->resource.length) {
			uni->nick.data = uni->resource.data + 1;
			uni->nick.length = uni->resource.length;
		}

	} else return PSYC_PARSE_UNIFORM_INVALID_SCHEME;

  if (uni->host.length == 0)
		return PSYC_PARSE_UNIFORM_INVALID_HOST;

	uni->valid = 1;
	return uni->type;
}
