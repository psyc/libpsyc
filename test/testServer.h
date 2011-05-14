#ifndef TESTSERVER_H
# define TESTSERVER_H

# define RECV_BUF_SIZE 8 * 1024
# define CONT_BUF_SIZE 8 * 1024
# define SEND_BUF_SIZE 8 * 1024
# define NUM_PARSERS 100

char sendbuf[SEND_BUF_SIZE];
char buf[CONT_BUF_SIZE + RECV_BUF_SIZE];  // cont buf + recv buf: [  ccrrrr]
char *recvbuf = buf + CONT_BUF_SIZE;      // recv buf:                 ^^^^
                                          // parse buf:              ^^^^^^

// cmd line args
uint8_t verbose, routing_only, parse_multiple, no_render, progress, stats;
size_t nbytes;

void test_init(int i);
int test_input(int i);

#endif
