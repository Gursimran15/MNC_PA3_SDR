#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <map>
using namespace std;
typedef enum {FALSE, TRUE} bl;

#define MAX_ROUTERS 5
struct router_init{
 uint16_t router_id;
  uint16_t r_port;
   uint16_t d_port;
    uint16_t cost;
     uint16_t next_hop;
    char ip[20];
};
struct routing_tab{
 uint16_t router_id;
    uint16_t cost;
     uint16_t next_hop;
};
extern char HOSTIP[20];
extern uint16_t MYID;
extern map <int,int>router_table;
extern map <int,int>next_hop;
extern uint16_t NUM_ROUTERS;
extern uint16_t TIME_PERIOD;
#define ERROR(err_msg) {perror(err_msg); exit(EXIT_FAILURE);}

/* https://scaryreasoner.wordpress.com/2009/02/28/checking-sizeof-at-compile-time/ */
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)])) // Interesting stuff to read if you are interested to know how this works

extern uint16_t CONTROL_PORT;

#endif