#ifndef TESTSERVER_H
# define TESTSERVER_H

# define RECV_BUF_SIZE 8 * 1024
# define CONT_BUF_SIZE 8 * 1024
# define SEND_BUF_SIZE 8 * 1024
# define NUM_PARSERS 100

void test_init(int i);
int test_input(int i, char *recvbuf, size_t nbytes);
void test_server(const char* port, size_t recv_buf_size);

#endif
