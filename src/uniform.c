#include <ctype.h>
#include "lib.h"
#include "psyc/uniform.h"
#include "psyc/parse.h"

int psyc_parseUniform2 (psycUniform *uni, const char *str, size_t length)
{
	char c;
	psycString *p;
	size_t pos = 0, part = PSYC_UNIFORM_SCHEME, scheme;

	(*uni)[PSYC_UNIFORM_FULL].ptr = str;
	(*uni)[PSYC_UNIFORM_FULL].length = length;

	while (pos < length) {
		c = str[pos];
		if (c == ':') {
			(*uni)[part].ptr = str;
			(*uni)[part].length = pos++;
			break;
		} else if (!psyc_isHostChar(c))
			return PSYC_PARSE_UNIFORM_INVALID_SCHEME;
		pos++;
	}

	p = &(*uni)[PSYC_UNIFORM_SCHEME];
	if (p->length == 4 &&
	    tolower(p->ptr[0]) == 'p' &&
	    tolower(p->ptr[1]) == 's' &&
	    tolower(p->ptr[2]) == 'y' &&
	    tolower(p->ptr[3]) == 'c') {
		scheme = PSYC_SCHEME_PSYC;
		part = PSYC_UNIFORM_SLASHES;
		(*uni)[part].ptr = str + pos;
		(*uni)[part].length = 0;

		while (pos < length) {
			c = str[pos];
			switch (part) {
				case PSYC_UNIFORM_SLASHES:
					if (c == '/')
						(*uni)[part].length++;
					else return PSYC_PARSE_UNIFORM_INVALID_SLASHES;

					if ((*uni)[part].length == 2) {
						part = PSYC_UNIFORM_HOST;
						(*uni)[part].ptr = str + pos + 1;
						(*uni)[part].length = 0;
					}
					break;

				case PSYC_UNIFORM_HOST:
					if (psyc_isHostChar(c)) {
						(*uni)[part].length++;
						break;
					}

					if ((*uni)[part].length == 0)
						return PSYC_PARSE_UNIFORM_INVALID_HOST;

					if (c == ':')
						part = PSYC_UNIFORM_PORT;
					else if (c == '/')
						part = PSYC_UNIFORM_RESOURCE;
					else return PSYC_PARSE_UNIFORM_INVALID_HOST;

					(*uni)[part].ptr = str + pos + 1;
					(*uni)[part].length = 0;
					break;

				case PSYC_UNIFORM_PORT:
					if (psyc_isNumeric(c)) {
						(*uni)[part].length++;
						break;
					}

					if ((*uni)[part].length == 0 && c != PSYC_TRANSPORT_GNUNET)
						return PSYC_PARSE_UNIFORM_INVALID_PORT;

					if (c == '/') {
						part = PSYC_UNIFORM_RESOURCE;
						(*uni)[part].ptr = str + pos + 1;
						(*uni)[part].length = 0;
						break;
					}
					else {
						part = PSYC_UNIFORM_TRANSPORT;
						(*uni)[part].ptr = str + pos;
						(*uni)[part].length = 0;
					}
					// fall thru

				case PSYC_UNIFORM_TRANSPORT:
					switch (c) {
						case PSYC_TRANSPORT_GNUNET:
							if ((*uni)[PSYC_UNIFORM_PORT].length > 0)
								return PSYC_PARSE_UNIFORM_INVALID_TRANSPORT;
						case PSYC_TRANSPORT_TCP:
						case PSYC_TRANSPORT_UDP:
						case PSYC_TRANSPORT_TLS:
							if ((*uni)[part].length > 0)
								return PSYC_PARSE_UNIFORM_INVALID_TRANSPORT;
							(*uni)[part].length++;
							break;
						case '/':
							part = PSYC_UNIFORM_RESOURCE;
							(*uni)[part].ptr = str + pos + 1;
							(*uni)[part].length = 0;
							break;
						default:
							return PSYC_PARSE_UNIFORM_INVALID_TRANSPORT;
					}
					break;

				case PSYC_UNIFORM_RESOURCE:
					if (psyc_isNameChar(c)) {
						(*uni)[part].length++;
						break;
					} else if (c == '#') {
						part = PSYC_UNIFORM_CHANNEL;
						(*uni)[part].ptr = str + pos + 1;
						(*uni)[part].length = 0;
						break;
					} else return PSYC_PARSE_UNIFORM_INVALID_RESOURCE;

				case PSYC_UNIFORM_CHANNEL:
					if (psyc_isNameChar(c)) {
						(*uni)[part].length++;
						break;
					} else return PSYC_PARSE_UNIFORM_INVALID_CHANNEL;
			}
			pos++;
		}

		if ((*uni)[PSYC_UNIFORM_HOST].length == 0)
			return PSYC_PARSE_UNIFORM_INVALID_HOST;

		(*uni)[PSYC_UNIFORM_ROOT].ptr = str;
		(*uni)[PSYC_UNIFORM_ROOT].length = (*uni)[PSYC_UNIFORM_SCHEME].length + 1 +
			(*uni)[PSYC_UNIFORM_SLASHES].length + (*uni)[PSYC_UNIFORM_HOST].length +
			(*uni)[PSYC_UNIFORM_PORT].length + (*uni)[PSYC_UNIFORM_TRANSPORT].length;

		if ((*uni)[PSYC_UNIFORM_PORT].length > 0 ||
		    (*uni)[PSYC_UNIFORM_TRANSPORT].length > 0)
			(*uni)[PSYC_UNIFORM_ROOT].length++; // :

		(*uni)[PSYC_UNIFORM_BODY].ptr = (*uni)[PSYC_UNIFORM_HOST].ptr;
		(*uni)[PSYC_UNIFORM_BODY].length = length - (*uni)[PSYC_UNIFORM_SCHEME].length - 1 -
			(*uni)[PSYC_UNIFORM_SLASHES].length;

		if ((*uni)[PSYC_UNIFORM_RESOURCE].length) {
			(*uni)[PSYC_UNIFORM_NICK].ptr = (*uni)[PSYC_UNIFORM_RESOURCE].ptr + 1;
			(*uni)[PSYC_UNIFORM_NICK].length = (*uni)[PSYC_UNIFORM_RESOURCE].length;
		}
	} else return PSYC_PARSE_UNIFORM_INVALID_SCHEME;

  if ((*uni)[PSYC_UNIFORM_HOST].length == 0)
		return PSYC_PARSE_UNIFORM_INVALID_HOST;

	return scheme;
}

int psyc_parseUniform (psycUniform *uni, psycString *str)
{
	return psyc_parseUniform2(uni, str->ptr, str->length);
}
