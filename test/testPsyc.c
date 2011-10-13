#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/socket.h>

#include <psyc.h>
#include <psyc/parse.h>
#include <psyc/render.h>
#include <psyc/syntax.h>

#include "test.c"

// max size for routing & entity header
#define ROUTING_LINES 16
#define ENTITY_LINES 32

// cmd line args
char *filename, *port = "4440";
uint8_t verbose, stats;
uint8_t multiple, single, routing_only, no_render, quiet, progress;
size_t count = 1, recv_buf_size;

psycParseState parsers[NUM_PARSERS];
psycPacket packets[NUM_PARSERS];
psycModifier routing[NUM_PARSERS][ROUTING_LINES];
psycModifier entity[NUM_PARSERS][ENTITY_LINES];

int contbytes, exit_code;

static inline
void resetString (psycString *s, uint8_t freeptr);

// initialize parser & packet variables
void test_init (int i) {
	// reset parser state & packet
	if (routing_only)
		psyc_initParseState2(&parsers[i], PSYC_PARSE_ROUTING_ONLY);
	else
		psyc_initParseState(&parsers[i]);

	memset(&packets[i], 0, sizeof(psycPacket));
	memset(&routing[i], 0, sizeof(psycModifier) * ROUTING_LINES);
	memset(&entity[i], 0, sizeof(psycModifier) * ENTITY_LINES);
	packets[i].routing.modifiers = routing[i];
	packets[i].entity.modifiers = entity[i];
}

// parse & render input
int test_input (int i, char *recvbuf, size_t nbytes) {
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

	psycParseState *parser = &parsers[i];
	psycPacket *packet = &packets[i];

	char oper;
	psycString name, value, elem;
	psycString *pname = NULL, *pvalue = NULL;
	psycModifier *mod = NULL;
	psycParseListState listState;
	size_t len;

	// Set buffer with data for the parser.
	psyc_setParseBuffer2(parser, parsebuf, contbytes + nbytes);
	contbytes = 0;
	oper = 0;
	name.length = 0;
	value.length = 0;

	do {
		if (verbose >= 3)
			printf("\n# buffer = [%.*s]\n# part = %d\n", (int)parser->buffer.length, parser->buffer.ptr, parser->part);
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
					mod->flag = psyc_isParseValueLengthFound(parser) ?
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
					packet->flag = psyc_isParseContentLengthFound(parser) ?
						PSYC_PACKET_NEED_LENGTH : PSYC_PACKET_NO_LENGTH;

					if (routing_only) {
						packet->content = packet->data;
						resetString(&(packet->data), 0);
					}

					psyc_setPacketLength(packet);

					if (psyc_render(packet, sendbuf, SEND_BUF_SIZE) == PSYC_RENDER_SUCCESS) {
						if (!quiet) {
							if (filename && write(1, sendbuf, packet->length) == -1) {
								perror("write");
								ret = -1;
							} else if (!filename && send(i, sendbuf, packet->length, 0) == -1) {
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
				packet->routingLength = 0;
				packet->contentLength = 0;
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

				contbytes = psyc_getParseRemainingLength(parser);

				if (contbytes > 0) { // copy end of parsebuf before start of recvbuf
					if (verbose >= 3)
						printf("# remaining = [%.*s]\n", (int)contbytes, psyc_getParseRemainingBuffer(parser));
					assert(contbytes <= CONT_BUF_SIZE); // make sure it's still in the buffer
					memmove(recvbuf - contbytes, psyc_getParseRemainingBuffer(parser), contbytes);
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
					pname->ptr = malloc(name.length);
					pname->length = name.length;

					assert(pname->ptr != NULL);
					memcpy((void*)pname->ptr, name.ptr, name.length);
					name.length = 0;

					if (verbose >= 2)
						printf("%.*s = ", (int)pname->length, pname->ptr);
				}

				if (value.length) {
					if (!pvalue->length) {
						if (psyc_isParseValueLengthFound(parser))
							len = psyc_getParseValueLength(parser);
						else
							len = value.length;
						pvalue->ptr = malloc(len);
					}
					assert(pvalue->ptr != NULL);
					memcpy((void*)pvalue->ptr + pvalue->length, value.ptr, value.length);
					pvalue->length += value.length;
					value.length = 0;

					if (verbose >= 2) {
						printf("[%.*s]", (int)pvalue->length, pvalue->ptr);
						if (parser->valueLength > pvalue->length)
							printf("...");
						printf("\n");
					}
				}
				else if (verbose)
					printf("\n");

				if (verbose >= 3)
					printf("\t\t\t\t\t\t\t\t# n:%ld v:%ld c:%ld r:%ld\n",
					       pname->length, pvalue->length,
					       parser->contentParsed, parser->routingLength);
		}

		switch (ret) {
			case PSYC_PARSE_ROUTING:
			case PSYC_PARSE_ENTITY:
			case PSYC_PARSE_ENTITY_END:
				oper = 0;
				name.length = 0;
				value.length = 0;

				if (psyc_isListVar(pname)) {
					if (verbose >= 2)
						printf("## LIST START\n");

					psyc_initParseListState(&listState);
					psyc_setParseListBuffer(&listState, *pvalue);

					do {
						retl = psyc_parseList(&listState, &elem);
						switch (retl) {
							case PSYC_PARSE_LIST_END:
								retl = 0;
							case PSYC_PARSE_LIST_ELEM:
								if (verbose >= 2) {
									printf("|%.*s\n", (int)elem.length, elem.ptr);
									if (ret == PSYC_PARSE_LIST_END)
										printf("## LIST END");
								}
								break;

							default:
								printf("# Error while parsing list: %i\n", retl);
								ret = retl = -1;
						}
					}
					while (retl > 0);
				}
		}
	}
	while (ret > 0);

	if (progress)
		r = write(1, " ", 1);

	return ret;
}

static inline
void resetString (psycString *s, uint8_t freeptr)
{
	if (freeptr && s->length)
		free((void*)s->ptr);

	s->ptr = NULL;
	s->length = 0;
}

int main (int argc, char **argv) {
	int c;
	while ((c = getopt (argc, argv, "f:p:b:c:mnqrsvPSh")) != -1) {
		switch (c) {
			CASE_f CASE_p CASE_b CASE_c
			CASE_m CASE_n CASE_q CASE_r
			CASE_s CASE_v CASE_S CASE_P
			case 'h':
				printf(
					HELP_FILE("testPsyc", "mnqrSsvP")
					HELP_PORT("testPsyc", "nqrsvP")
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
