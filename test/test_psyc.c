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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/socket.h>

#include <psyc.h>

#include "test.c"

// max size for routing & entity header
#define ROUTING_LINES 16
#define ENTITY_LINES 32

// cmd line args
char *filename, *port = "4440";
uint8_t verbose, stats;
uint8_t multiple, single, routing_only, no_render, quiet, progress;
size_t count = 1, recv_buf_size;

PsycParseState parsers[NUM_PARSERS];
PsycPacket packets[NUM_PARSERS];
PsycModifier routing[NUM_PARSERS][ROUTING_LINES];
PsycModifier entity[NUM_PARSERS][ENTITY_LINES];

int contbytes, exit_code;

static inline void
resetString (PsycString *s, uint8_t freeptr);

// initialize parser & packet variables
void
test_init (int i)
{
    // reset parser state & packet
    psyc_parse_state_init(&parsers[i],
			  routing_only ? PSYC_PARSE_ROUTING_ONLY : PSYC_PARSE_ALL);

    memset(&packets[i], 0, sizeof(PsycPacket));
    memset(&routing[i], 0, sizeof(PsycModifier) * ROUTING_LINES);
    memset(&entity[i], 0, sizeof(PsycModifier) * ENTITY_LINES);
    packets[i].routing.modifiers = routing[i];
    packets[i].entity.modifiers = entity[i];
}

// parse & render input
int
test_input (int i, char *recvbuf, size_t nbytes)
{
    int j, ret, retl, r;
    char sendbuf[SEND_BUF_SIZE];
    char *parsebuf = recvbuf - contbytes;
    /* We have a buffer with pointers pointing to various parts of it:
     *   *contbuf-vv
     * buffer: [  ccrrrr]
     *   *recvbuf---^^^^
     *  *parsebuf-^^^^^^
     *
     * New data is in recvbuf, if it contains an incomplete packet then remaining
     * unparsed data is copied to contbuf that will be parsed during the next call
     * to this function together with the new data.
     */

    PsycParseState *parser = &parsers[i];
    PsycPacket *packet = &packets[i];

    char oper;
    PsycString name, value, type;
    PsycString *pname = NULL, *pvalue = NULL;
    PsycModifier *mod = NULL;
    PsycParseListState lstate;
    PsycParseDictState dstate;
    size_t len;

    // Set buffer with data for the parser.
    psyc_parse_buffer_set(parser, parsebuf, contbytes + nbytes);
    contbytes = 0;
    oper = 0;
    name.length = 0;
    value.length = 0;

    do {
	if (verbose >= 3)
	    printf("\n# buffer = [%.*s]\n# part = %d\n",
		   (int)parser->buffer.length, parser->buffer.data, parser->part);
	// Parse the next part of the packet (a routing/entity modifier or the body)
	ret = exit_code = psyc_parse(parser, &oper, &name, &value);
	if (verbose >= 2)
	    printf("# ret = %d\n", ret);

	switch (ret) {
	case PSYC_PARSE_ROUTING:
	    assert(packet->routing.lines < ROUTING_LINES);
	    mod = &(packet->routing.modifiers[packet->routing.lines]);
	    pname = &mod->name;
	    pvalue = &mod->value;
	    mod->flag = PSYC_MODIFIER_ROUTING;
	    packet->routing.lines++;
	    break;

	case PSYC_PARSE_STATE_RESYNC:
	case PSYC_PARSE_STATE_RESET:
	    packet->stateop = oper;
	    break;

	case PSYC_PARSE_ENTITY_START:
	case PSYC_PARSE_ENTITY_CONT:
	case PSYC_PARSE_ENTITY_END:
	case PSYC_PARSE_ENTITY:
	    assert(packet->entity.lines < ENTITY_LINES);
	    mod = &(packet->entity.modifiers[packet->entity.lines]);
	    pname = &mod->name;
	    pvalue = &mod->value;

	    if (ret == PSYC_PARSE_ENTITY || ret == PSYC_PARSE_ENTITY_END) {
		packet->entity.lines++;
		mod->flag = psyc_parse_value_length_found(parser) ?
		    PSYC_MODIFIER_NEED_LENGTH : PSYC_MODIFIER_NO_LENGTH;
	    }
	    break;

	case PSYC_PARSE_BODY_START:
	case PSYC_PARSE_BODY_CONT:
	case PSYC_PARSE_BODY_END:
	case PSYC_PARSE_BODY:
	    pname = &(packet->method);
	    pvalue = &(packet->data);
	    break;

	case PSYC_PARSE_COMPLETE:
	    if (verbose)
		printf("# Done parsing.\n");
	    else if (progress)
		r = write(1, ".", 1);
	    if ((filename && !multiple) || (!filename && single))
		ret = -1;

	    if (!no_render) {
		packet->flag = psyc_parse_content_length_found(parser) ?
		    PSYC_PACKET_NEED_LENGTH : PSYC_PACKET_NO_LENGTH;

		if (routing_only) {
		    packet->content = packet->data;
		    resetString(&(packet->data), 0);
		}

		psyc_packet_length_set(packet);

		if (PSYC_RENDER_SUCCESS == psyc_render(packet, sendbuf,
						       SEND_BUF_SIZE)) {
		    if (!quiet) {
			if (filename && write(1, sendbuf, packet->length) == -1) {
			    perror("write");
			    ret = -1;
			} else if (!filename && -1 == send(i, sendbuf,
							   packet->length, 0)) {
			    perror("send");
			    ret = -1;
			}
		    }
		} else {
		    printf("# Render error");
		    ret = -1;
		}
	    }

	    // reset packet
	    packet->routinglen = 0;
	    packet->contentlen = 0;
	    packet->length = 0;
	    packet->flag = 0;

	    for (j = 0; j < packet->routing.lines; j++) {
		resetString(&(packet->routing.modifiers[j].name), 1);
		resetString(&(packet->routing.modifiers[j].value), 1);
	    }
	    packet->routing.lines = 0;

	    if (routing_only) {
		resetString(&(packet->content), 1);
	    } else {
		for (j = 0; j < packet->entity.lines; j++) {
		    resetString(&(packet->entity.modifiers[j].name), 1);
		    resetString(&(packet->entity.modifiers[j].value), 1);
		}
		packet->entity.lines = 0;

		resetString(&(packet->method), 1);
		resetString(&(packet->data), 1);
	    }

	    break;

	case PSYC_PARSE_INSUFFICIENT:
	    if (verbose >= 2)
		printf("# Insufficient data.\n");

	    contbytes = psyc_parse_remaining_length(parser);

	    if (contbytes > 0) { // copy end of parsebuf before start of recvbuf
		if (verbose >= 3)
		    printf("# remaining = [%.*s]\n",
			   (int)contbytes, psyc_parse_remaining_buffer(parser));
		assert(contbytes <= CONT_BUF_SIZE); // make sure it fits in the buffer
		memmove(recvbuf - contbytes,
			psyc_parse_remaining_buffer(parser), contbytes);
	    }
	    ret = 0;
	    break;

	default:
	    printf("# Error while parsing: %i\n", ret);
	    ret = -1;
	}

	switch (ret) {
	case PSYC_PARSE_ENTITY_START:
	case PSYC_PARSE_ENTITY_CONT:
	case PSYC_PARSE_BODY_START:
	case PSYC_PARSE_BODY_CONT:
	    ret = 0;
	case PSYC_PARSE_ENTITY:
	case PSYC_PARSE_ENTITY_END:
	case PSYC_PARSE_ROUTING:
	case PSYC_PARSE_BODY:
	case PSYC_PARSE_BODY_END:
	    if (oper) {
		mod->oper = oper;
		if (verbose >= 2)
		    printf("%c", oper);
	    }

	    if (name.length) {
		pname->data = malloc(name.length);
		pname->length = name.length;

		assert(pname->data != NULL);
		memcpy((void*)pname->data, name.data, name.length);
		name.length = 0;

		if (verbose >= 2)
		    printf("%.*s = ", (int)pname->length, pname->data);
	    }

	    if (value.length) {
		if (!pvalue->length) {
		    if (psyc_parse_value_length_found(parser))
			len = psyc_parse_value_length(parser);
		    else
			len = value.length;
		    pvalue->data = malloc(len);
		}
		assert(pvalue->data != NULL);
		memcpy((void*)pvalue->data + pvalue->length, value.data, value.length);
		pvalue->length += value.length;
		value.length = 0;

		if (verbose >= 2) {
		    printf("[%.*s]", (int)pvalue->length, pvalue->data);
		    if (parser->valuelen > pvalue->length)
			printf("...");
		    printf("\n");
		}
	    }
	    else if (verbose)
		printf("\n");

	    if (verbose >= 3)
		printf("\t\t\t\t\t\t\t\t# n:%ld v:%ld c:%ld r:%ld\n",
		       pname->length, pvalue->length,
		       parser->content_parsed, parser->routinglen);
	}

	switch (ret) {
	case PSYC_PARSE_ROUTING:
	case PSYC_PARSE_ENTITY:
	case PSYC_PARSE_ENTITY_END:
	    oper = 0;
	    name.length = 0;
	    value.length = 0;
	    type.length = 0;

	    switch (psyc_var_type(PSYC_S2ARG(*pname))) {
	    case PSYC_TYPE_LIST:
		if (verbose >= 2)
		    printf("## LIST START\n");

		psyc_parse_list_state_init(&lstate);
		psyc_parse_list_buffer_set(&lstate, PSYC_S2ARG(*pvalue));

		do {
		    retl = psyc_parse_list(&lstate, &type, &value);
		    switch (retl) {
		    case PSYC_PARSE_LIST_TYPE:
			if (verbose >= 2)
			    printf("## LIST TYPE: %.*s\n", (int)type.length, type.data);
			break;
		    case PSYC_PARSE_LIST_ELEM_START:
		    case PSYC_PARSE_LIST_ELEM_LAST:
			retl = 0;
		    case PSYC_PARSE_LIST_ELEM:
			if (verbose >= 2) {
			    printf("|%.*s [%.*s]", (int)type.length, type.data,
				   (int)value.length, value.data);
			    if (retl == PSYC_PARSE_LIST_ELEM_START)
				printf(" ...");
			    printf("\n");
			    if (ret == PSYC_PARSE_LIST_ELEM_LAST)
				printf("## LAST ELEM\n");
			}
			break;
		    case PSYC_PARSE_LIST_ELEM_CONT:
			retl = 0;
		    case PSYC_PARSE_LIST_ELEM_END:
			if (verbose >= 2) {
			    printf("... [%.*s]", (int)value.length, value.data);
			    if (retl == PSYC_PARSE_LIST_ELEM_CONT)
				printf(" ...");
			    printf("\n");
			}
			break;
		    case PSYC_PARSE_LIST_END:
			retl = 0;
			if (verbose >= 2)
			    printf("## LIST END\n");
			break;
		    default:
			printf("# Error while parsing list: %i\n", retl);
			ret = retl = -1;
		    }
		} while (retl > 0);
		break;
	    case PSYC_TYPE_DICT:
		if (verbose >= 2)
		    printf("## DICT START\n");

		psyc_parse_dict_state_init(&dstate);
		psyc_parse_dict_buffer_set(&dstate, PSYC_S2ARG(*pvalue));

		do {
		    retl = psyc_parse_dict(&dstate, &type, &value);
		    switch (retl) {
		    case PSYC_PARSE_DICT_TYPE:
			if (verbose >= 2)
			    printf("## DICT TYPE: %.*s\n", (int)type.length, type.data);
			break;
		    case PSYC_PARSE_DICT_KEY_START:
			retl = 0;
		    case PSYC_PARSE_DICT_KEY:
			if (verbose >= 2) {
			    printf("{[%.*s]", (int)value.length, value.data);
			    if (retl == PSYC_PARSE_DICT_KEY_START)
				printf(" ...");
			    printf("\n");
			}
			break;
		    case PSYC_PARSE_DICT_KEY_CONT:
			retl = 0;
		    case PSYC_PARSE_DICT_KEY_END:
			if (verbose >= 2) {
			    printf("... [%.*s]", (int)value.length, value.data);
			    if (retl == PSYC_PARSE_DICT_KEY_CONT)
				printf(" ...");
			    printf("\n");
			}
			break;
		    case PSYC_PARSE_DICT_VALUE_START:
		    case PSYC_PARSE_DICT_VALUE_LAST:
			retl = 0;
		    case PSYC_PARSE_DICT_VALUE:
			if (verbose >= 2) {
			    printf("}%.*s [%.*s]", (int)type.length, type.data,
				   (int)value.length, value.data);
			    if (retl == PSYC_PARSE_DICT_VALUE_START)
				printf(" ...");
			    printf("\n");
			    if (ret == PSYC_PARSE_DICT_VALUE_LAST)
				printf("## LAST VALUE\n");
			}
			break;
		    case PSYC_PARSE_DICT_VALUE_CONT:
			retl = 0;
		    case PSYC_PARSE_DICT_VALUE_END:
			if (verbose >= 2) {
			    printf("... [%.*s]", (int)value.length, value.data);
			    if (retl == PSYC_PARSE_DICT_VALUE_CONT)
				printf(" ...");
			    printf("\n");
			}
			break;
		    case PSYC_PARSE_DICT_END:
			retl = 0;
			printf("## DICT END\n");
			break;
		    default:
			printf("# Error while parsing dict: %i\n", retl);
			ret = retl = -1;
		    }
		} while (retl > 0);
		break;
	    default:
		break;
	    }
	}
    } while (ret > 0);

    if (progress)
	r = write(1, " ", 1);

    return ret;
}

static inline void
resetString (PsycString *s, uint8_t freeptr)
{
    if (freeptr && s->length)
	free((void*)s->data);

    s->data = NULL;
    s->length = 0;
}

int
main (int argc, char **argv)
{
    int c;
    while ((c = getopt (argc, argv, "f:p:b:c:mnqrsvPSh")) != -1) {
	switch (c) {
	CASE_f CASE_p CASE_b CASE_c
	CASE_m CASE_n CASE_q CASE_r
	CASE_s CASE_v CASE_S CASE_P
	case 'h':
	    printf(HELP_FILE("test_psyc", "mnqrSsvP")
		   HELP_PORT("test_psyc", "nqrsvP")
		   HELP_f HELP_p HELP_b HELP_c
		   HELP_m HELP_n HELP_r
		   HELP_q HELP_S HELP_s
		   HELP_v HELP_P HELP_h,
		   port, RECV_BUF_SIZE);
	    exit(0);
	case '?': exit(-1);
	default:  abort();
	}
    }

    if (filename)
	test_file(filename, count, recv_buf_size);
    else
	test_server(port, count, recv_buf_size);

    return exit_code;
}
