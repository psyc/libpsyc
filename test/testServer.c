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
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define __USE_POSIX
#include <netdb.h>
#include <math.h>
#include <assert.h>

#include <psyc.h>
#include <psyc/parser.h>
#include <psyc/render.h>
#include <psyc/syntax.h>

const size_t RECV_BUF_SIZE = 8 * 1024;
const size_t CONT_BUF_SIZE = 8 * 1024;
const size_t SEND_BUF_SIZE = 8 * 1024;
const size_t NUM_PARSERS = 100;
// max size for routing & entity header
const size_t ROUTING_LINES = 16;
const size_t ENTITY_LINES = 32;

static inline
void resetString (psycString *s, uint8_t freeptr)
{
	if (freeptr && s->length)
		free((void*)s->ptr);

	s->ptr = NULL;
	s->length = 0;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr (struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main (int argc, char **argv)
{
	char *port = argc > 1 ? argv[1] : "4440";
	uint8_t routing_only   = argc > 2 && memchr(argv[2], (int)'r', strlen(argv[2]));
	uint8_t verbose        = argc > 2 && memchr(argv[2], (int)'v', strlen(argv[2]));
	uint8_t parse_multiple = argc > 2 && memchr(argv[2], (int)'m', strlen(argv[2]));
	uint8_t progress       = argc > 2 && memchr(argv[2], (int)'p', strlen(argv[2]));
	uint8_t stats          = argc > 2 && memchr(argv[2], (int)'s', strlen(argv[2]));
	size_t recv_buf_size   = argc > 3 ? atoi(argv[3]) : 0;
	if (recv_buf_size <= 0)
		recv_buf_size = RECV_BUF_SIZE;

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
	int i, j, rv;

	struct addrinfo hints, *ai, *p;

	psycParseState parsers[NUM_PARSERS];
	psycPacket packets[NUM_PARSERS];
	psycModifier routing[NUM_PARSERS][ROUTING_LINES];
	psycModifier entity[NUM_PARSERS][ENTITY_LINES];
	psycModifier *mod = NULL;

	struct timeval start[NUM_PARSERS], end[NUM_PARSERS];

	int ret, retl;
	char oper;
	psycString name, value, elem;
	psycString *pname = NULL, *pvalue = NULL;
	psycParseListState listState;

	FD_ZERO(&master);    // clear the master and temp sets
	FD_ZERO(&read_fds);

	// get us a socket and bind it
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if ((rv = getaddrinfo(NULL, port, &hints, &ai)) != 0)
{
		fprintf(stderr, "error: %s\n", gai_strerror(rv));
		exit(1);
	}
	
	for (p = ai; p != NULL; p = p->ai_next)
	{
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0)
			continue;
		
		// lose the pesky "address already in use" error message
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)
		{
			close(listener);
			continue;
		}

		break;
	}

	// if we got here, it means we didn't get bound
	if (p == NULL)
	{
		fprintf(stderr, "failed to bind\n");
		exit(2);
	}

	freeaddrinfo(ai); // all done with this

	// listen
	if (listen(listener, 10) == -1)
	{
		perror("listen");
		exit(3);
	}

	// add the listener to the master set
	FD_SET(listener, &master);

	// keep track of the biggest file descriptor
	fdmax = listener; // so far, it's this one

	// main loop
	for (;;)
	{
		read_fds = master; // copy it
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
		{
			perror("select");
			exit(4);
		}

		// run through the existing connections looking for data to read
		for (i = 0; i <= fdmax; i++)
		{
			if (FD_ISSET(i, &read_fds)) // we got one!!
			{
				if (i == listener)
				{
					// handle new connections
					if (fdmax == NUM_PARSERS - 1)
						continue; // ignore if there's too many

					addrlen = sizeof remoteaddr;
					newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);

					if (newfd == -1)
						perror("accept");
					else
					{
						FD_SET(newfd, &master); // add to master set
						if (newfd > fdmax) // keep track of the max
							fdmax = newfd;

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

						if (stats)
							gettimeofday(&start[newfd], NULL);
					}
				}
				else
				{
					// handle data from a client
					if ((nbytes = recv(i, recvbuf, recv_buf_size, 0)) <= 0)
					{
						if (stats)
							printf("%ld ms\n", (end[i].tv_sec * 1000000 + end[i].tv_usec - start[i].tv_sec * 1000000 - start[i].tv_usec) / 1000);

						// got error or connection closed by client
						if (nbytes == 0) // connection closed
							printf("socket %d hung up\n", i);
						else
							perror("recv");

						close(i); // bye!
						FD_CLR(i, &master); // remove from master set
					}
					else
					{
						// we got some data from a client
						parsebuf = recvbuf - contbytes;
						psyc_setParseBuffer(&parsers[i], psyc_newString(parsebuf, contbytes + nbytes));
						contbytes = 0;
						oper = 0;
						name.length = 0;
						value.length = 0;

						do
						{
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

									if (ret == PSYC_PARSE_ENTITY)
									{
										packets[i].entity.lines++;
										mod->flag = psyc_isParseValueLengthFound(&parsers[i]) ?
											PSYC_MODIFIER_NEED_LENGTH : PSYC_MODIFIER_NO_LENGTH;
									}
									break;

								case PSYC_PARSE_BODY_INCOMPLETE:
								case PSYC_PARSE_BODY:
									pname = &(packets[i].method);
									pvalue = &(packets[i].data);
									break;

								case PSYC_PARSE_COMPLETE:
									if (verbose)
										printf("# Done parsing.\n");
									else if (progress)
										write(1, ".", 1);
									if (!parse_multiple) // parse multiple packets?
										ret = -1;

									packets[i].flag = psyc_isParseContentLengthFound(&parsers[i]) ?
										PSYC_PACKET_NEED_LENGTH : PSYC_PACKET_NO_LENGTH;

									if (routing_only)
									{
										packets[i].content = packets[i].data;
										resetString(&packets[i].data, 0);
									}

									psyc_setPacketLength(&packets[i]);

									if (psyc_render(&packets[i], sendbuf, SEND_BUF_SIZE) == PSYC_RENDER_SUCCESS)
									{
										if (send(i, sendbuf, packets[i].length, 0) == -1)
										{
											perror("send error");
											ret = -1;
										}
									}
									else
									{
										perror("render error");
										ret = -1;
									}

									// reset packet
									packets[i].routingLength = 0;
									packets[i].contentLength = 0;
									packets[i].length = 0;
									packets[i].flag = 0;

									for (j = 0; j < packets[i].routing.lines; j++)
									{
										resetString(&packets[i].routing.modifiers[j].name, 1);
										resetString(&packets[i].routing.modifiers[j].value, 1);
									}
									packets[i].routing.lines = 0;

									if (routing_only)
									{
										resetString(&packets[i].content, 1);
									}
									else
									{
										for (j = 0; j < packets[i].entity.lines; j++)
										{
											resetString(&packets[i].entity.modifiers[j].name, 1);
											resetString(&packets[i].entity.modifiers[j].value, 1);
										}
										packets[i].entity.lines = 0;

										resetString(&packets[i].method, 1);
										resetString(&packets[i].data, 1);
									}

									break;

								case PSYC_PARSE_INSUFFICIENT:
									if (verbose)
										printf("# Insufficient data.\n");

									contbytes = psyc_getParseRemainingLength(&parsers[i]);

									if (contbytes > 0) // copy end of parsebuf before start of recvbuf
									{
										assert(recvbuf - contbytes >= buf); // make sure it's still in the buffer
										memmove(recvbuf - contbytes, psyc_getParseRemainingBuffer(&parsers[i]), contbytes);
									}
									ret = 0;
									break;

								default:
									printf("# Error while parsing: %i\n", ret);
									ret = -1;
							}

							switch (ret)
							{
								case PSYC_PARSE_ENTITY_INCOMPLETE:
								case PSYC_PARSE_BODY_INCOMPLETE:
									ret = 0;
								case PSYC_PARSE_ENTITY:
								case PSYC_PARSE_ROUTING:
								case PSYC_PARSE_BODY:
									if (oper)
									{
										mod->oper = oper;
										if (verbose)
											printf("%c", oper);
									}

									if (name.length)
									{
										pname->ptr = malloc(name.length);
										pname->length = name.length;

										assert(pname->ptr != NULL);
										memcpy((void*)pname->ptr, name.ptr, name.length);
										name.length = 0;

										if (verbose)
											printf("%.*s = ", (int)pname->length, pname->ptr);
									}

									if (value.length) {
										if (!pvalue->length)
											pvalue->ptr = malloc(parsers[i].valueLength ? parsers[i].valueLength : value.length);
										assert(pvalue->ptr != NULL);
										memcpy((void*)pvalue->ptr + pvalue->length, value.ptr, value.length);
										pvalue->length += value.length;
										value.length = 0;

										if (verbose)
										{
											printf("[%.*s]", (int)pvalue->length, pvalue->ptr);
											if (parsers[i].valueLength > pvalue->length)
												printf("...");
											printf("\n");
										}
									}
									else if (verbose)
										printf("\n");

									if (verbose)
										printf("\t\t\t\t\t\t\t\t# n:%ld v:%ld c:%ld r:%ld\n",
										       pname->length, pvalue->length,
										       parsers[i].contentParsed, parsers[i].routingLength);
							}

							switch (ret)
							{
								case PSYC_PARSE_ROUTING:
								case PSYC_PARSE_ENTITY:
									if (pname->length >= 5 && memcmp(pname->ptr, "_list", 5) == 0)
									{
										if (verbose)
											printf("## LIST START\n");

										psyc_initParseListState(&listState);
										psyc_setParseListBuffer(&listState, *pvalue);

										do
										{
											retl = psyc_parseList(&listState, pname, pvalue, &elem);
											switch (retl)
											{
												case PSYC_PARSE_LIST_END:
													retl = 0;
												case PSYC_PARSE_LIST_ELEM:
													if (verbose)
													{
														printf("|%.*s\n", (int)elem.length, elem.ptr);
														if (ret == PSYC_PARSE_LIST_END)
															printf("## LIST END");
													}
													break;

												default:
													printf("# Error while parsing list: %i\n", ret);
													ret = retl = -1;
											}
										}
										while (retl > 0);
									}
							}
						}
						while (ret > 0);

						if (progress)
							write(1, " ", 1);

						if (stats)
							gettimeofday(&end[i], NULL);

						if (ret < 0)
						{
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
