#ifndef TEST_H
# define TEST_H

#ifndef RECV_BUF_SIZE
# define RECV_BUF_SIZE 8 * 1024
#endif
#ifndef CONT_BUF_SIZE
# define CONT_BUF_SIZE 8 * 1024
#endif
#ifndef SEND_BUF_SIZE
# define SEND_BUF_SIZE 8 * 1024
#endif
#ifndef NUM_PARSERS
# define NUM_PARSERS 100
#endif

void test_init(int i);
int test_input(int i, char *recvbuf, size_t nbytes);

void test_file(const char* filename, size_t count, size_t recv_buf_size);
void test_server(const char* port, size_t count, size_t recv_buf_size);
void check_range(char c, const char *s, int min, int max);

#endif
