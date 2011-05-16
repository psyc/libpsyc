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

#define CASE_f case 'f': filename = optarg; break;
#define CASE_p case 'p': port = optarg; check_range(c, optarg, 1, 0); break;
#define CASE_b case 'b': recv_buf_size = atoi(optarg); check_range(c, optarg, 1, RECV_BUF_SIZE); break;
#define CASE_c case 'c': count = atoi(optarg); check_range(c, optarg, 1, 0); break;
#define CASE_n case 'n': no_render = 1; break;
#define CASE_m case 'm': multiple = 1; break;
#define CASE_q case 'q': quiet = 1; break;
#define CASE_r case 'r': routing_only = 1; break;
#define CASE_s case 's': stats = 1; break;
#define CASE_v case 'v': verbose++; break;
#define CASE_P case 'P': progress = 1; break;
#define CASE_S case 'S': single = 1; break;
#define HELP_FILE(name, opts)	name " -f <filename> [-b <read_buf_size>] [-c <count>] [-" opts "]\n"
#define HELP_PORT(name, opts)	name " [-p <port>] [-b <recv_buf_size>] [-" opts "]\n"
#define HELP_f "  -f <filename>\tInput file name\n"
#define HELP_p "  -p <port>\t\tListen on TCP port, default is %s\n"
#define HELP_b "  -b <buf_size>\tRead/receive buffer size, default is %d\n"
#define HELP_c "  -c <count>\t\tParse data from file <count> times\n"
#define HELP_m "  -m\t\t\tParse multiple packets from file\n"
#define HELP_n "  -n\t\t\tNo rendering, only parsing\n"
#define HELP_q "  -q\t\t\tQuiet mode, don't output rendered string\n"
#define HELP_r "  -r\t\t\tParse routing header only\n"
#define HELP_S "  -S\t\t\tSingle packet mode, close connection after parsing one packet\n"
#define HELP_s "  -s\t\t\tShow statistics at the end\n"
#define HELP_v "  -v\t\t\tVerbose, can be specified multiple times for more verbosity\n"
#define HELP_P "  -P\t\t\tShow progress\n"
#define HELP_h "  -h\t\t\tShow this help\n"

void test_init(int i);
int test_input(int i, char *recvbuf, size_t nbytes);

void test_file(const char* filename, size_t count, size_t recv_buf_size);
void test_server(const char* port, size_t count, size_t recv_buf_size);
void check_range(char c, const char *s, int min, int max);

#endif
