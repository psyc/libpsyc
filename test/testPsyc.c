#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/socket.h>

#include <psyc.h>
#include <psyc/parse.h>
#include <psyc/render.h>
#include <psyc/syntax.h>

#include "test.h"

// max size for routing & entity header
#define ROUTING_LINES 16
#define ENTITY_LINES 32

// cmd line args
uint8_t verbose, stats, file;
uint8_t routing_only, parse_multiple, no_render, progress;

psycParseState parsers[NUM_PARSERS];
psycPacket packets[NUM_PARSERS];
psycModifier routing[NUM_PARSERS][ROUTING_LINES];
psycModifier entity[NUM_PARSERS][ENTITY_LINES];

int contbytes = 0, last_ret = 0;

int main (int argc, char **argv) {
	char *port = argc > 1 ? argv[1] : "4440";
	char *opts = argc > 2 ? argv[2] : NULL;
	char *v, *w;
	verbose = opts && (v = memchr(opts, (int)'v', strlen(opts))) ?
		v - opts > 0 && (w = memchr(v+1, (int)'v', strlen(opts) - (v - opts))) ?
		w - v > 0 && memchr(w+1, (int)'v', strlen(opts) - (w - opts)) ? 3 : 2 : 1 : 0;
	stats          = opts && memchr(opts, (int)'s', strlen(opts));

	routing_only   = opts && memchr(opts, (int)'r', strlen(opts));
	parse_multiple = opts && memchr(opts, (int)'m', strlen(opts));
	no_render      = opts && memchr(opts, (int)'n', strlen(opts));
	progress       = opts && memchr(opts, (int)'p', strlen(opts));
	file           = opts && memchr(opts, (int)'f', strlen(opts));
	size_t recv_buf_size   = argc > 3 ? atoi(argv[3]) : 0;

	if (file)
		test_file(argv[1], recv_buf_size);
	else
		test_server(port, recv_buf_size);

	return last_ret;
}

static inline
void resetString (psycString *s, uint8_t freeptr) {
	if (freeptr && s->length)
		free((void*)s->ptr);

	s->ptr = NULL;
	s->length = 0;
}

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

int test_input (int i, char *recvbuf, size_t nbytes) {
	int j, ret, retl, r;
	char *parsebuf = recvbuf - contbytes;
	char sendbuf[SEND_BUF_SIZE];

	char oper;
	psycString name, value, elem;
	psycString *pname = NULL, *pvalue = NULL;
	psycModifier *mod = NULL;
	psycParseListState listState;
	size_t len;

	psyc_setParseBuffer2(&parsers[i], parsebuf, contbytes + nbytes);
	contbytes = 0;
	oper = 0;
	name.length = 0;
	value.length = 0;

	do {
		ret = last_ret = psyc_parse(&parsers[i], &oper, &name, &value);
		if (verbose >= 2)
			printf("# ret = %d\n", ret);

		switch (ret) {
			case PSYC_PARSE_ROUTING:
				assert(packets[i].routing.lines < ROUTING_LINES);
				mod = &(packets[i].routing.modifiers[packets[i].routing.lines]);
				pname = &mod->name;
				pvalue = &mod->value;
				mod->flag = PSYC_MODIFIER_ROUTING;
				packets[i].routing.lines++;
				break;

			case PSYC_PARSE_ENTITY_START:
			case PSYC_PARSE_ENTITY_CONT:
			case PSYC_PARSE_ENTITY_END:
			case PSYC_PARSE_ENTITY:
				assert(packets[i].entity.lines < ENTITY_LINES);
				mod = &(packets[i].entity.modifiers[packets[i].entity.lines]);
				pname = &mod->name;
				pvalue = &mod->value;

				if (ret == PSYC_PARSE_ENTITY || ret == PSYC_PARSE_ENTITY_END) {
					packets[i].entity.lines++;
					mod->flag = psyc_isParseValueLengthFound(&parsers[i]) ?
						PSYC_MODIFIER_NEED_LENGTH : PSYC_MODIFIER_NO_LENGTH;
				}
				break;

			case PSYC_PARSE_BODY_START:
			case PSYC_PARSE_BODY_CONT:
			case PSYC_PARSE_BODY_END:
			case PSYC_PARSE_BODY:
				pname = &(packets[i].method);
				pvalue = &(packets[i].data);
				break;

			case PSYC_PARSE_COMPLETE:
				if (verbose)
					printf("# Done parsing.\n");
				else if (progress)
					r = write(1, ".", 1);
				if (!parse_multiple) // parse multiple packets?
					ret = -1;

				if (!no_render) {
					packets[i].flag = psyc_isParseContentLengthFound(&parsers[i]) ?
						PSYC_PACKET_NEED_LENGTH : PSYC_PACKET_NO_LENGTH;

					if (routing_only) {
						packets[i].content = packets[i].data;
						resetString(&packets[i].data, 0);
					}

					psyc_setPacketLength(&packets[i]);

					if (psyc_render(&packets[i], sendbuf, SEND_BUF_SIZE) == PSYC_RENDER_SUCCESS) {
						if (file && write(1, sendbuf, packets[i].length) == -1) {
							perror("write");
							ret = -1;
						} else if (!file && send(i, sendbuf, packets[i].length, 0) == -1) {
							perror("send");
							ret = -1;
						}
					} else {
						printf("# Render error");
						ret = -1;
					}
				}

				// reset packet
				packets[i].routingLength = 0;
				packets[i].contentLength = 0;
				packets[i].length = 0;
				packets[i].flag = 0;

				for (j = 0; j < packets[i].routing.lines; j++) {
					resetString(&packets[i].routing.modifiers[j].name, 1);
					resetString(&packets[i].routing.modifiers[j].value, 1);
				}
				packets[i].routing.lines = 0;

				if (routing_only) {
					resetString(&packets[i].content, 1);
				} else {
					for (j = 0; j < packets[i].entity.lines; j++) {
						resetString(&packets[i].entity.modifiers[j].name, 1);
						resetString(&packets[i].entity.modifiers[j].value, 1);
					}
					packets[i].entity.lines = 0;

					resetString(&packets[i].method, 1);
					resetString(&packets[i].data, 1);
				}

				break;

			case PSYC_PARSE_INSUFFICIENT:
				if (verbose >= 2)
					printf("# Insufficient data.\n");

				contbytes = psyc_getParseRemainingLength(&parsers[i]);

				if (contbytes > 0) { // copy end of parsebuf before start of recvbuf
					assert(contbytes <= CONT_BUF_SIZE); // make sure it's still in the buffer
					memmove(recvbuf - contbytes, psyc_getParseRemainingBuffer(&parsers[i]), contbytes);
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
						if (psyc_isParseValueLengthFound(&parsers[i]))
							len = psyc_getParseValueLength(&parsers[i]);
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
						if (parsers[i].valueLength > pvalue->length)
							printf("...");
						printf("\n");
					}
				}
				else if (verbose)
					printf("\n");

				if (verbose >= 3)
					printf("\t\t\t\t\t\t\t\t# n:%ld v:%ld c:%ld r:%ld\n",
					       pname->length, pvalue->length,
					       parsers[i].contentParsed, parsers[i].routingLength);
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
