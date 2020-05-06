#ifndef GLOBAL_H_
#define GLOBAL_H_


#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;
typedef enum {FALSE, TRUE} bl;

#define MAX_ROUTERS 5
struct router_init{
 uint16_t router_id;
  uint16_t r_port;
   uint16_t d_port;
    uint16_t cost;
    uint32_t ip;
};
struct routing_tab_payload{
    uint16_t router_id;
    uint16_t padding;
     uint16_t next_hop;
     uint16_t cost;
};
struct router_table{
int router_id;
int cost;
int next_hop;
bool neighbour;
};
struct CONTROL_PAYLOAD{
uint16_t num_r;
uint16_t u_intv;
// uint16_t *routers;
// uint16_t router_id[MAX_ROUTERS];
// uint16_t r_port[MAX_ROUTERS];
// uint16_t d_port[MAX_ROUTERS];
// uint16_t cost[MAX_ROUTERS];
// char ip[MAX_ROUTERS][INET_ADDRSTRLEN];
};
typedef struct router_table ROUTER_TABLE;
extern char HOSTIP[20];
extern uint16_t MYID;
extern ROUTER_TABLE rt[MAX_ROUTERS];
extern uint16_t NUM_ROUTERS;
extern uint16_t TIME_PERIOD;
extern uint16_t ROUTER_PORT;
extern uint16_t DATA_PORT;
// extern bool init_rt;
extern fd_set master_list, watch_list;
extern int head_fd;
#define ERROR(err_msg) {perror(err_msg); exit(EXIT_FAILURE);}

/* https://scaryreasoner.wordpress.com/2009/02/28/checking-sizeof-at-compile-time/ */
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)])) // Interesting stuff to read if you are interested to know how this works

extern uint16_t CONTROL_PORT;
void log_print(char* filename, int line, char *fmt,...);
char* print_time();
#define LOG_PRINT(...) log_print(__FILE__, __LINE__, __VA_ARGS__ )

#endif