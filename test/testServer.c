/**
 * libpsyc test server for packet parsing & rendering
 *
 * based on selectserver.c from http://beej.us/guide/bgnet/
 *	"The C source code presented in this document is hereby granted to the public domain, and is completely free of any license restriction."
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <math.h>
#include <assert.h>

#include <psyc.h>
#include <psyc/parser.h>
#include <psyc/render.h>
#include <psyc/syntax.h>

const size_t RECV_BUF_SIZE = 256;
const size_t CONT_BUF_SIZE = 512;
const size_t SEND_BUF_SIZE = 1024;
const size_t NUM_PARSERS = 100;
// max size for routing & entity header
const size_t ROUTING_LINES = 16;
const size_t ENTITY_LINES = 32;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char **argv)
{
	char *port = argc > 1 ? argv[1] : "4440";
	uint8_t routing_only = argc > 2, verbose = argc > 3;

	fd_set master;    // master file descriptor list
	fd_set read_fds;  // temp file descriptor list for select()
	int fdmax;        // maximum file descriptor number

	int listener;     // listening socket descriptor
	int newfd;        // newly accept()ed socket descriptor
	struct sockaddr_storage remoteaddr; // client address
	socklen_t addrlen;

	char buf[CONT_BUF_SIZE + RECV_BUF_SIZE];  // cont buf + recv buf: [  ccrrrr]
	char *recvbuf = buf + CONT_BUF_SIZE;      // recv buf:                 ^^^^
	char *parsebuf;                           // parse buf:              ^^^^^^
	char sendbuf[SEND_BUF_SIZE];
	size_t nbytes, contbytes = 0;

	char remoteIP[INET6_ADDRSTRLEN];

	int yes = 1;        // for setsockopt() SO_REUSEADDR, below
	int i, rv;

	struct addrinfo hints, *ai, *p;

	psycParseState parsers[NUM_PARSERS];
	psycPacket packets[NUM_PARSERS];
	psycModifier routing[NUM_PARSERS][ROUTING_LINES];
	psycModifier entity[NUM_PARSERS][ENTITY_LINES];
	psycModifier *mod;

	int ret, retl;
	char oper;
	psycString name, value, elem;
	psycString *pname, *pvalue;
	psycParseListState listState;

	FD_ZERO(&master);    // clear the master and temp sets
	FD_ZERO(&read_fds);

	// get us a socket and bind it
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if ((rv = getaddrinfo(NULL, port, &hints, &ai)) != 0) {
		fprintf(stderr, "error: %s\n", gai_strerror(rv));
		exit(1);
	}
	
	for (p = ai; p != NULL; p = p->ai_next) {
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0) { 
			continue;
		}
		
		// lose the pesky "address already in use" error message
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
			close(listener);
			continue;
		}

		break;
	}

	// if we got here, it means we didn't get bound
	if (p == NULL) {
		fprintf(stderr, "failed to bind\n");
		exit(2);
	}

	freeaddrinfo(ai); // all done with this

	// listen
	if (listen(listener, 10) == -1) {
		perror("listen");
		exit(3);
	}

	// add the listener to the master set
	FD_SET(listener, &master);

	// keep track of the biggest file descriptor
	fdmax = listener; // so far, it's this one

	// main loop
	for (;;) {
		read_fds = master; // copy it
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(4);
		}

		// run through the existing connections looking for data to read
		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) { // we got one!!
				if (i == listener) {
					// handle new connections
					if (fdmax == NUM_PARSERS - 1)
						continue; // ignore if there's too many

					addrlen = sizeof remoteaddr;
					newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);

					if (newfd == -1) {
						perror("accept");
					} else {
						FD_SET(newfd, &master); // add to master set
						if (newfd > fdmax) {    // keep track of the max
							fdmax = newfd;
						}

						// reset parser state & packet
						if (routing_only)
							psyc_initParseState2(&parsers[newfd], PSYC_PARSE_ROUTING_ONLY);
						else
							psyc_initParseState(&parsers[newfd]);

						memset(&packets[newfd], 0, sizeof(psycPacket));
						memset(&routing[newfd], 0, sizeof(psycModifier) * ROUTING_LINES);
						memset(&entity[newfd], 0, sizeof(psycModifier) * ENTITY_LINES);
						packets[newfd].routing.modifiers = routing[newfd];
						packets[newfd].entity.modifiers = entity[newfd];

						printf("# New connection from %s on socket %d\n",
						       inet_ntop(remoteaddr.ss_family,
						                 get_in_addr((struct sockaddr*)&remoteaddr),
						                 remoteIP, INET6_ADDRSTRLEN),
						       newfd);
					}
				} else {
					// handle data from a client
					if ((nbytes = recv(i, recvbuf, RECV_BUF_SIZE, 0)) <= 0) {
						// got error or connection closed by client
						if (nbytes == 0) {
							// connection closed
							printf("socket %d hung up\n", i);
						} else {
							perror("recv");
						}
						close(i); // bye!
						FD_CLR(i, &master); // remove from master set
					} else {
						// we got some data from a client
						parsebuf = recvbuf - contbytes;
						psyc_setParseBuffer(&parsers[i], psyc_newString(parsebuf, contbytes + nbytes));
						contbytes = 0;
						oper = 0;
						name.length = 0;
						value.length = 0;

						do {
							ret = psyc_parse(&parsers[i], &oper, &name, &value);
							if (verbose)
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
								case PSYC_PARSE_ENTITY_INCOMPLETE:
								case PSYC_PARSE_ENTITY:
									assert(packets[i].entity.lines < ENTITY_LINES);
									mod = &(packets[i].entity.modifiers[packets[i].entity.lines]);
									pname = &mod->name;
									pvalue = &mod->value;
									if (ret == PSYC_PARSE_ENTITY) {
										packets[i].entity.lines++;
										mod->flag = parsers[i].valueLength ? PSYC_MODIFIER_NEED_LENGTH : PSYC_MODIFIER_NO_LENGTH;
									}
									break;
								case PSYC_PARSE_BODY_INCOMPLETE:
								case PSYC_PARSE_BODY:
									pname = &(packets[i].method);
									pvalue = &(packets[i].data);
									break;
								case PSYC_PARSE_COMPLETE:
									printf("# Done parsing.\n");
									packets[i].flag = parsers[i].contentLengthFound ? PSYC_PACKET_NEED_LENGTH : PSYC_PACKET_NO_LENGTH;
									psyc_setPacketLength(&packets[i]);
									psyc_render(&packets[i], sendbuf, SEND_BUF_SIZE);
									if (send(i, sendbuf, packets[i].length, 0) == -1) {
										perror("send");
									}
									ret = -1;
									break;
								case PSYC_PARSE_INSUFFICIENT:
									if (verbose)
										printf("# Insufficient data.\n");
									contbytes = parsers[i].buffer.length - parsers[i].cursor;
									if (contbytes > 0) { // copy end of parsebuf before start of recvbuf
										assert(recvbuf - contbytes >= buf); // make sure it's still in the buffer
										memcpy(recvbuf - contbytes, parsebuf + parsers[i].cursor, contbytes);
									}
									ret = 0;
									break;
								default:
									printf("# Error while parsing: %i\n", ret);
									ret = -1;
							}

							switch (ret) {
								case PSYC_PARSE_ENTITY_INCOMPLETE:
								case PSYC_PARSE_BODY_INCOMPLETE:
									ret = 0;
								case PSYC_PARSE_ENTITY:
								case PSYC_PARSE_ROUTING:
								case PSYC_PARSE_BODY:
									if (oper) {
										mod->oper = oper;
										if (verbose)
											printf("%c", oper);
									}

									if (name.length) {
										pname->ptr = malloc(name.length);
										pname->length = name.length;
										assert(pname->ptr != NULL);
										memcpy((void*)pname->ptr, name.ptr, name.length);
										name.length = 0;
										if (verbose) {
											printf("%.*s = ", (int)pname->length, pname->ptr);
										}
									}

									if (value.length) {
										if (!pvalue->ptr)
											pvalue->ptr = malloc(parsers[i].valueLength ? parsers[i].valueLength : value.length);
										assert(pvalue->ptr != NULL);
										memcpy((void*)pvalue->ptr + pvalue->length, value.ptr, value.length);
										pvalue->length += value.length;
										value.length = 0;
										if (verbose) {
											printf("%.*s", (int)pvalue->length, pvalue->ptr);
											if (parsers[i].valueLength > pvalue->length)
												printf("...");
											printf("\n");
										}
									} else if (verbose) {
										printf("\n");
									}

									if (verbose)
										printf("\t\t\t\t\t\t\t\t# n:%ld v:%ld c:%ld r:%ld\n", pname->length, pvalue->length, parsers[i].contentParsed, parsers[i].routingLength);
							}

							switch (ret) {
								case PSYC_PARSE_ROUTING:
								case PSYC_PARSE_ENTITY:
									if (pname->length >= 5 && memcmp(pname->ptr, "_list", 5) == 0) {
										if (verbose)
											printf("## LIST START\n");

										psyc_initParseListState(&listState);
										psyc_setParseListBuffer(&listState, *pvalue);

										do {
											retl = psyc_parseList(&listState, pname, pvalue, &elem);
											switch (retl) {
												case PSYC_PARSE_LIST_END:
													retl = 0;
												case PSYC_PARSE_LIST_ELEM:
													if (verbose) {
														printf("|%.*s\n", (int)elem.length, elem.ptr);
														if (ret == PSYC_PARSE_LIST_END)
															printf("## LIST END");
													}
													break;
												default:
													printf("# Error while parsing list: %i\n", ret);
													ret = retl = -1;
											}
										} while (retl > 0);
									}
							}
						} while (ret > 0);
						if (ret < 0) {
							printf("# Closing connection: %i\n", i);
							close(i); // bye!
							FD_CLR(i, &master); // remove from master set
						}
					}
				} // END handle data from client
			} // END got new incoming connection
		} // END looping through file descriptors
	} // END for(;;)--and you thought it would never end!

	return 0;
}
