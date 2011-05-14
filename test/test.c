/**
 * test server for packet parsing & rendering
 *
 * based on selectserver.c from http://beej.us/guide/bgnet/
 *	"The C source code presented in this document is hereby granted to the public domain, and is completely free of any license restriction."
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#define __USE_POSIX
#include <netdb.h>

#include "test.h"

// cmd line args
extern uint8_t verbose, stats;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr (struct sockaddr *sa) {
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void test_file(const char* filename, size_t recv_buf_size) {
	if (recv_buf_size <= 0)
		recv_buf_size = RECV_BUF_SIZE;

	char buf[CONT_BUF_SIZE + RECV_BUF_SIZE];  // cont buf + recv buf: [  ccrrrr]
	char *recvbuf = buf + CONT_BUF_SIZE;      // recv buf:                 ^^^^
	size_t nbytes;

	int fd = open(filename, O_RDONLY);
	if (fd < 0) {
		perror("open");
		exit(1);
	}

	test_init(0);

	while ((nbytes = read(fd, (void*)recvbuf, recv_buf_size)))
		test_input(0, recvbuf, nbytes);
}

void test_server(const char* port, size_t recv_buf_size) {
	if (recv_buf_size <= 0)
		recv_buf_size = RECV_BUF_SIZE;

	char buf[CONT_BUF_SIZE + RECV_BUF_SIZE];  // cont buf + recv buf: [  ccrrrr]
	char *recvbuf = buf + CONT_BUF_SIZE;      // recv buf:                 ^^^^

	fd_set master;    // master file descriptor list
	fd_set read_fds;  // temp file descriptor list for select()
	int fdmax;        // maximum file descriptor number

	int listener;     // listening socket descriptor
	int newfd;        // newly accept()ed socket descriptor
	struct sockaddr_storage remoteaddr; // client address
	socklen_t addrlen;
	size_t nbytes;

	char remoteIP[INET6_ADDRSTRLEN];

	int yes = 1;        // for setsockopt() SO_REUSEADDR, below
	int i, rv, ret;

	struct addrinfo hints, *ai, *p;
	struct timeval start[NUM_PARSERS], end[NUM_PARSERS];

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
		if (listener < 0)
			continue;
		
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
						if (newfd > fdmax) // keep track of the max
							fdmax = newfd;

						test_init(newfd);

						if (verbose)
							printf("# New connection from %s on socket %d\n",
						         inet_ntop(remoteaddr.ss_family,
						                   get_in_addr((struct sockaddr*)&remoteaddr),
						                   remoteIP, INET6_ADDRSTRLEN),
							       newfd);

						if (stats)
							gettimeofday(&start[newfd], NULL);
					}
				} else {
					// handle data from a client
					if ((nbytes = recv(i, recvbuf, recv_buf_size, 0)) <= 0) {
						if (stats)
							printf("%ld ms\n", (end[i].tv_sec * 1000000 + end[i].tv_usec - start[i].tv_sec * 1000000 - start[i].tv_usec) / 1000);

						// got error or connection closed by client
						if (nbytes == 0) { // connection closed
							if (verbose)
								printf("# Socket %d hung up\n", i);
						} else {
							perror("recv");
						}

						close(i); // bye!
						FD_CLR(i, &master); // remove from master set
					} else {
						// we got some data from a client
						if (verbose >= 2)
							printf("> %ld bytes\n", nbytes);
						if (verbose >= 3)
							printf("> [%.*s]", (int)nbytes, recvbuf);

						ret = test_input(i, recvbuf, nbytes);

						if (stats)
							gettimeofday(&end[i], NULL);

						if (ret < 0) {
							if (verbose)
								printf("# Closing connection: %i\n", i);
							close(i); // bye!
							FD_CLR(i, &master); // remove from master set
						}
					}
				} // END handle data from client
			} // END got new incoming connection
		} // END looping through file descriptors
	} // END for(;;)--and you thought it would never end!
}
