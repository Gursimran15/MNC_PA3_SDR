/**
 * @gursimr2_assignment3
 * @author  Gursimran Singh <gursimr2@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/queue.h>
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include <iterator>
using namespace std;
//FROM HEADERS
#define CNTRL_HEADER_SIZE 8
#define CNTRL_RESP_HEADER_SIZE 8

#define PACKET_USING_STRUCT // Comment this out to use alternate packet crafting technique

#ifdef PACKET_USING_STRUCT
    struct __attribute__((__packed__)) CONTROL_HEADER
    {
        uint32_t dest_ip_addr;
        uint8_t control_code;
        uint8_t response_time;
        uint16_t payload_len;
    };

    struct __attribute__((__packed__)) CONTROL_RESPONSE_HEADER
    {
        uint32_t controller_ip_addr;
        uint8_t control_code;
        uint8_t response_code;
        uint16_t payload_len;
    };
#endif
//int control_socket, router_socket, data_socket;

// GLOBAL
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
uint16_t router_id;
uint16_t cost;
uint16_t next_hop;
bl neighbour;
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
char *dv_payload=0;
typedef struct router_table ROUTER_TABLE;
uint32_t HOSTIP;
uint16_t MYID;
ROUTER_TABLE rt[MAX_ROUTERS];
router_init rt_init[MAX_ROUTERS];
uint16_t NUM_ROUTERS;
uint16_t TIME_PERIOD = 0;
uint16_t ROUTER_PORT;
uint16_t DATA_PORT;
fd_set master_list, watch_list;
int head_fd;
int control_socket, router_socket, data_socket;
// extern bool init_rt;
//fd_set master_list, watch_list;
//int head_fd;
#define ERROR(err_msg) {perror(err_msg); exit(EXIT_FAILURE);}
/* https://scaryreasoner.wordpress.com/2009/02/28/checking-sizeof-at-compile-time/ */
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)])) // Interesting stuff to read if you are interested to know how this works

uint16_t CONTROL_PORT;
// GLOBAL

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
//Logging
void log_print(char* filename, int line, char *fmt,...);
#define LOG_PRINT(...) log_print(__FILE__, __LINE__, __VA_ARGS__ )

/* logger.c */
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include "../include/logger.h"
FILE *fp ;
static int SESSION_TRACKER; //Keeps track of session
std::string logname = "/tmp/gursimr2.txt";

char* print_time()
{
    int size = 0;
    time_t t;
    char *buf;
    
    t=time(NULL); /* get current calendar time */
    
    char *timestr = asctime( localtime(&t) );
    timestr[strlen(timestr) - 1] = 0;  //Getting rid of \n
    
    size = strlen(timestr)+ 1 + 2; //Additional +2 for square braces
    buf = (char*)malloc(size);
    
    memset(buf, 0x0, size);
    snprintf(buf,size,"[%s]", timestr);
   
    return buf;
}

void log_print(char* filename, int line, char *fmt,...)
{
    va_list list;
    char *p, *r;
    int e;

    
    if(SESSION_TRACKER > 0)
      fp = fopen (logname.c_str(),"a+");
    else {
      fp = fopen (logname.c_str(),"w");
    }
    fprintf(fp,"%s ",print_time());
    fprintf(fp,"[%s][line: %d] ",filename,line);
    va_start( list, fmt );

    for ( p = fmt ; *p ; ++p )
    {
        if ( *p != '%' )//If simple string
        {
            fputc( *p,fp );
        }
        else
        {
            switch ( *++p )
            {
                /* string */
            case 's':
            {
                r = va_arg( list, char * );

                fprintf(fp,"%s", r);
                continue;
            }

            /* integer */
            case 'd':
            {
                e = va_arg( list, int );

                fprintf(fp,"%d", e);
                continue;
            }

            default:
                fputc( *p, fp );
            }
        }
    }
    va_end( list );
    fputc( '\n', fp );
    SESSION_TRACKER++;
    fclose(fp);
}
//CONTROL Header

#ifndef PACKET_USING_STRUCT
    #define CNTRL_RESP_CONTROL_CODE_OFFSET 0x04
    #define CNTRL_RESP_RESPONSE_CODE_OFFSET 0x05
    #define CNTRL_RESP_PAYLOAD_LEN_OFFSET 0x06
#endif

char* create_response_header(int sock_index, uint8_t control_code, uint8_t response_code, uint16_t payload_len)
{
    char *buffer;
    #ifdef PACKET_USING_STRUCT
        /** ASSERT(sizeof(struct CONTROL_RESPONSE_HEADER) == 8) 
          * This is not really necessary with the __packed__ directive supplied during declaration (see control_header_lib.h).
          * If this fails, comment #define PACKET_USING_STRUCT in control_header_lib.h
          */
        BUILD_BUG_ON(sizeof(struct CONTROL_RESPONSE_HEADER) != CNTRL_RESP_HEADER_SIZE); // This will FAIL during compilation itself; See comment above.

        struct CONTROL_RESPONSE_HEADER *cntrl_resp_header;
    #endif
    #ifndef PACKET_USING_STRUCT
        char *cntrl_resp_header;
    #endif

    struct sockaddr_in addr;
    socklen_t addr_size;

    buffer = (char *) malloc(sizeof(char)*CNTRL_RESP_HEADER_SIZE);
    #ifdef PACKET_USING_STRUCT
        cntrl_resp_header = (struct CONTROL_RESPONSE_HEADER *) buffer;
    #endif
    #ifndef PACKET_USING_STRUCT
        cntrl_resp_header = buffer;
    #endif

    addr_size = sizeof(struct sockaddr_in);
    getpeername(sock_index, (struct sockaddr *)&addr, &addr_size);

    #ifdef PACKET_USING_STRUCT
        /* Controller IP Address */
        memcpy(&(cntrl_resp_header->controller_ip_addr), &(addr.sin_addr), sizeof(struct in_addr));
        /* Control Code */
        cntrl_resp_header->control_code = control_code;
        /* Response Code */
        cntrl_resp_header->response_code = response_code;
        /* Payload Length */
        cntrl_resp_header->payload_len = htons(payload_len);
    #endif

    #ifndef PACKET_USING_STRUCT
        /* Controller IP Address */
        memcpy(cntrl_resp_header, &(addr.sin_addr), sizeof(struct in_addr));
        /* Control Code */
        memcpy(cntrl_resp_header+CNTRL_RESP_CONTROL_CODE_OFFSET, &control_code, sizeof(control_code));
        /* Response Code */
        memcpy(cntrl_resp_header+CNTRL_RESP_RESPONSE_CODE_OFFSET, &response_code, sizeof(response_code));
        /* Payload Length */
        payload_len = htons(payload_len);
        memcpy(cntrl_resp_header+CNTRL_RESP_PAYLOAD_LEN_OFFSET, &payload_len, sizeof(payload_len));
    #endif

    return buffer;
}
//Network Util
ssize_t recvALL(int sock_index, char *buffer, ssize_t nbytes)
{
    ssize_t bytes = 0;
    bytes = recv(sock_index, buffer, nbytes, 0);

    if(bytes == 0) return -1;
    while(bytes != nbytes)
        bytes += recv(sock_index, buffer+bytes, nbytes-bytes, 0);
    return bytes;
}

ssize_t sendALL(int sock_index, char *buffer, ssize_t nbytes)
{
    ssize_t bytes = 0;
    bytes = send(sock_index, buffer, nbytes, 0);

    if(bytes == 0) return -1;
    while(bytes != nbytes)
        bytes += send(sock_index, buffer+bytes, nbytes-bytes, 0);

    return bytes;
}
ssize_t	sendALLtoip(int sock_index, char *buffer, ssize_t nbytes,const struct sockaddr *dest_addr, socklen_t addrlen){
ssize_t bytes = 0;
    bytes = sendto(sock_index, buffer, nbytes, 0,dest_addr,addrlen);

    if(bytes == 0) return -1;
    while(bytes != nbytes)
        bytes += sendto(sock_index, buffer+bytes, nbytes-bytes, 0,dest_addr,addrlen);

    return bytes;
}
//AUTHOR
#define AUTHOR_STATEMENT "I, gursimr2, have read and understood the course academic integrity policy."

void author_response(int sock_index)
{
	uint16_t payload_len, response_len;
	char *cntrl_response_header, *cntrl_response_payload, *cntrl_response;

	payload_len = sizeof(AUTHOR_STATEMENT)-1; // Discount the NULL chararcter
	cntrl_response_payload = (char *) malloc(payload_len);
	memcpy(cntrl_response_payload, AUTHOR_STATEMENT, payload_len);

	cntrl_response_header = create_response_header(sock_index, 0, 0, payload_len);

	response_len = CNTRL_RESP_HEADER_SIZE+payload_len;
	cntrl_response = (char *) malloc(response_len);
	/* Copy Header */
	memcpy(cntrl_response, cntrl_response_header, CNTRL_RESP_HEADER_SIZE);
	free(cntrl_response_header);
	/* Copy Payload */
	memcpy(cntrl_response+CNTRL_RESP_HEADER_SIZE, cntrl_response_payload, payload_len);
	free(cntrl_response_payload);

	sendALL(sock_index, cntrl_response, response_len);

	free(cntrl_response);
}
//INIT
// char HOSTIP[20];
// uint16_t MYID;
// uint16_t NUM_ROUTERS;
// uint16_t TIME_PERIOD;
// uint16_t ROUTER_PORT;
// uint16_t DATA_PORT;
// ROUTER_TABLE rt[MAX_ROUTERS];
// #include "../include/logger.h"
// bool init_rt=false;
void init_response(int sock_index)
{
	uint16_t payload_len=0, response_len=0;
	char *cntrl_response_header, *cntrl_response_payload, *cntrl_response;
	LOG_PRINT("I am inside init response\n");
	// payload_len = 0; // Discount the NULL chararcter
	// cntrl_response_payload = 0;
	// memcpy(cntrl_response_payload, 0, payload_len);

	cntrl_response_header = create_response_header(sock_index, 1, 0, payload_len);

	response_len = CNTRL_RESP_HEADER_SIZE+payload_len;
	cntrl_response = (char *) malloc(response_len);
	/* Copy Header */
	memcpy(cntrl_response, cntrl_response_header, CNTRL_RESP_HEADER_SIZE);
	
	/* Copy Payload */
	// memcpy(cntrl_response+CNTRL_RESP_HEADER_SIZE, cntrl_response_payload, payload_len);
	// free(cntrl_response_payload);

	sendALL(sock_index, cntrl_response, response_len);
	LOG_PRINT("%d\n",response_len);
	LOG_PRINT("Init Response Sent\n");
	free(cntrl_response_header);
	free(cntrl_response);
	
}
void init_payload(char *cntrl_payload){
//save data from init control packet to routing table
// vector<router_init> r;
// LOG_PRINT("I am inside init payload\n");
struct CONTROL_PAYLOAD *p=(struct CONTROL_PAYLOAD *) cntrl_payload;
 uint16_t num_r;
  uint16_t time_p;
  char ip[INET_ADDRSTRLEN];
//   uint16_t *r;
  num_r=ntohs(p->num_r);
  time_p=ntohs(p->u_intv);
//    r = ntohs(p->routers);
//   r[0].r_port =  ntohs(p->r_port[0]);
NUM_ROUTERS = num_r;
TIME_PERIOD = time_p;
  cout<<NUM_ROUTERS<<"\n";
  cout<<TIME_PERIOD<<"\n";
  for(int i=0;i<num_r;i++){
	  cout<<"Routers struct will be here"<<i+1<<"\n";
	struct router_init *rti=(struct router_init *)(cntrl_payload + 4 + 12*i);
		// cout<<rti->router_id<<" "<<rti->cost<<"\n";
		// cout<<ntohs(rti->router_id)<<" "<<ntohs(rti->cost)<<"\n";
		if(ntohs(rti->cost) == 0){
			MYID=ntohs(rti->router_id);
			ROUTER_PORT=ntohs(rti->r_port);
			DATA_PORT = ntohs(rti->d_port);
            rt_init[i].router_id = rti->router_id;
            rt_init[i].r_port = rti->r_port;
            rt_init[i].d_port = rti->d_port;
            rt_init[i].cost = rti->cost;
            rt_init[i].ip = rti->ip;
            HOSTIP = rti->ip;
			rt[i].router_id = ntohs(rti->router_id);
			rt[i].cost=ntohs(rti->cost);
			rt[i].next_hop=ntohs(rti->router_id);
			rt[i].neighbour = FALSE;
		}else{
			if(ntohs(rti->cost) == 65535){
                 rt_init[i].router_id = rti->router_id;
            rt_init[i].r_port = rti->r_port;
            rt_init[i].d_port = rti->d_port;
            rt_init[i].cost = rti->cost;
            rt_init[i].ip = rti->ip;
				rt[i].router_id = ntohs(rti->router_id);
				rt[i].cost=ntohs(rti->cost);
				rt[i].next_hop=ntohs(rti->cost);
				rt[i].neighbour = FALSE;

			}else{
                 rt_init[i].router_id = rti->router_id;
            rt_init[i].r_port = rti->r_port;
            rt_init[i].d_port = rti->d_port;
            rt_init[i].cost = rti->cost;
            rt_init[i].ip = rti->ip;
				rt[i].router_id=ntohs(rti->router_id);
				rt[i].cost=ntohs(rti->cost);
				rt[i].next_hop=ntohs(rti->router_id);
				rt[i].neighbour = TRUE;

			}
		}
		cout<<rt[i].router_id<<"\n";
		cout<<rt[i].cost<<"\n";
		cout<<rt[i].next_hop<<"\n";
		cout<<rt[i].neighbour<<"\n";
        cout<<ntohs(rt_init[i].router_id)<<"\n";
        cout<<ntohs(rt_init[i].r_port)<<"\n";
        cout<<ntohs(rt_init[i].d_port)<<"\n";
        cout<<ntohs(rt_init[i].cost)<<"\n";
        cout<<ntohs(rt_init[i].ip)<<"\n";
         LOG_PRINT("RT SAVED");
        LOG_PRINT("%d\n",(rt[i].router_id));
        LOG_PRINT("%d\n",(rt[i].next_hop));
        LOG_PRINT("%d\n",(rt[i].neighbour));
        LOG_PRINT("%d\n",(rt[i].cost));
        LOG_PRINT("INIT INFO SAVED");
        LOG_PRINT("%d\n",ntohs(rt_init[i].router_id));
        LOG_PRINT("%d\n",ntohs(rt_init[i].r_port));
        LOG_PRINT("%d\n",ntohs(rt_init[i].d_port));
        LOG_PRINT("%d\n",ntohs(rt_init[i].cost));
        LOG_PRINT("%d\n",ntohs(rt_init[i].ip));
        LOG_PRINT("Neighbour?");
        LOG_PRINT("%d\n",rt[i].neighbour);
        // LOG_PRINT("%s\n",rt[i].neighbour);
  }

}
int create_router_sock()
{
    int sock;
    struct sockaddr_in router_addr;
    socklen_t addrlen = sizeof(router_addr);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
        ERROR("socket() failed");
    int a[] = {1};
    // /* Make socket re-usable */
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, a, sizeof(int)) < 0)
        ERROR("setsockopt() failed");
    bzero(&router_addr, sizeof(router_addr));

    router_addr.sin_family = AF_INET;
    router_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    router_addr.sin_port = htons(ROUTER_PORT);

    if(bind(sock, (struct sockaddr *)&router_addr, sizeof(router_addr)) < 0)
        ERROR("bind() failed");
    
    // if(listen(sock, 5) < 0)
    //     ERROR("listen() failed");

	
    return sock;
}
// //will move to data file
int create_data_sock()
{
    int sock;
    struct sockaddr_in data_addr;
    socklen_t addrlen = sizeof(data_addr);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
        ERROR("socket() failed");
    int a[] = {1};
    /* Make socket re-usable */
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, a, sizeof(int)) < 0)
        ERROR("setsockopt() failed");

    bzero(&data_addr, sizeof(data_addr));

    data_addr.sin_family = AF_INET;
    data_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    data_addr.sin_port = htons(DATA_PORT);

    if(bind(sock, (struct sockaddr *)&data_addr, sizeof(data_addr)) < 0)
        ERROR("bind() failed");

    if(listen(sock, 5) < 0)
        ERROR("listen() failed");

    // LIST_INIT(&control_conn_list);

    return sock;
}

// ROUTER TABLE
void rt_response(int sock_index)
{
	std::cout<<"I am here rt-response\n";
	uint16_t payload_len=0, response_len=0;
	char *cntrl_response_header, *cntrl_response_payload, *cntrl_response;
	uint16_t padding=0,tmp=0,o=0,num=NUM_ROUTERS;
    // char* payload= rt_payload(sock_index);
	//std::cout<<"I am here rt-payload in response- "<<payload<<"\n";
	payload_len = (sizeof(uint16_t))*4*NUM_ROUTERS;// Discount the NULL chararcter
	cntrl_response_payload = (char *) malloc(payload_len);
	cntrl_response_header = create_response_header(sock_index, 2, 0, payload_len);
	std::cout<<"I am here rt-payload length in response- "<<payload_len<<"\n";
	bzero(cntrl_response_payload,payload_len);
	for(int i=0;i<num;i++){
			cout<<rt[i].router_id<<"\n";
			cout<<rt[i].cost<<"\n";
			cout<<rt[i].next_hop<<"\n";
			cout<<rt[i].neighbour<<"\n";
			tmp=htons(rt[i].router_id);
			//tmp=rtresponse.router_id;
			memcpy(cntrl_response_payload+ o,&tmp,2);
			o+=2;
			cout<<ntohs(tmp)<<"\n";
			tmp=htons(padding);
			//tmp=rtresponse.padding;
			memcpy(cntrl_response_payload + o,&tmp,2);
			o+=2;
			cout<<ntohs(tmp)<<"\n";
			tmp=htons(rt[i].next_hop);
			//tmp=rtresponse.next_hop;
			memcpy(cntrl_response_payload + o,&tmp,2);
			o+=2;
			cout<<ntohs(tmp)<<"\n";
			tmp=htons(rt[i].cost);
			//tmp=rtresponse.cost;
			memcpy(cntrl_response_payload + o,&tmp,2);
			o+=2;
			cout<<ntohs(tmp)<<"\n";
			}
	cout<<cntrl_response_payload[0]<<cntrl_response_payload[1]<<"\n";
	response_len = CNTRL_RESP_HEADER_SIZE+payload_len;
	cntrl_response = (char *) malloc(response_len);
	bzero(cntrl_response,response_len);
	/* Copy Header */
	memcpy(cntrl_response, cntrl_response_header, CNTRL_RESP_HEADER_SIZE);
	free(cntrl_response_header);
	/* Copy Payload */
	memcpy(cntrl_response+CNTRL_RESP_HEADER_SIZE, cntrl_response_payload, payload_len);
	free(cntrl_response_payload);

	sendALL(sock_index, cntrl_response, response_len);

	free(cntrl_response);
}
void create_dv_payload(){
    LOG_PRINT("Inside dv pay");
uint16_t payload_len = ((sizeof(uint16_t))*4 + (sizeof(uint32_t)))*NUM_ROUTERS + 2*(sizeof(uint16_t)) + (sizeof(uint32_t));// Discount the NULL chararcter
	dv_payload = (char *) malloc(payload_len);
    uint16_t num=0,tmp=0,o=0,padding=0;
    num = NUM_ROUTERS;
    uint16_t rport,update_f;
    rport=htons(ROUTER_PORT);
    update_f = htons(NUM_ROUTERS);
    memcpy(dv_payload+ o,&update_f,2);
			o+=2;
			cout<<ntohs(update_f)<<"\n";
    memcpy(dv_payload+ o,&rport,2);
			o+=2;
			cout<<ntohs(rport)<<"\n";
    memcpy(dv_payload+ o,&HOSTIP,4);
			o+=4;
			cout<<ntohs(HOSTIP)<<"\n";
    for(int i=0;i<num;i++){
			tmp=htons(rt_init[i].ip);
			memcpy(dv_payload + o,&tmp,4);
			o+=4;
			cout<<ntohs(tmp)<<"\n";

			tmp=htons(rt_init[i].r_port);
			memcpy(dv_payload + o,&tmp,2);
			o+=2;
			cout<<ntohs(tmp)<<"\n";
            
            tmp=htons(padding);
			memcpy(dv_payload + o,&tmp,2);
			o+=2;
			cout<<ntohs(tmp)<<"\n";
            
            tmp=htons(rt_init[i].router_id);
			memcpy(dv_payload + o,&tmp,2);
			o+=2;
			cout<<ntohs(tmp)<<"\n";
            
            tmp=htons(rt_init[i].cost);
			memcpy(dv_payload + o,&tmp,2);
			o+=2;
			cout<<ntohs(tmp)<<"\n";
			}
            LOG_PRINT("Finish dv pay");
}
void dv_send(){
    uint16_t payload_len = ((sizeof(uint16_t))*4 + (sizeof(uint32_t)))*NUM_ROUTERS + 2*(sizeof(uint16_t)) + (sizeof(uint32_t));
for(int i=0;i<NUM_ROUTERS;i++){
     if(rt[i].neighbour == 1){
     int sock;
    struct sockaddr_in router_addr;
    socklen_t addrlen = sizeof(router_addr);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
        ERROR("socket() failed");

    bzero(&router_addr, sizeof(router_addr));

    router_addr.sin_family = AF_INET;
    router_addr.sin_addr.s_addr = rt_init[i].ip;
    router_addr.sin_port = rt_init[i].r_port;
	sendALLtoip(sock,dv_payload,payload_len,(struct sockaddr *)&router_addr,(socklen_t)sizeof(router_addr));
    close(sock);
    LOG_PRINT("Inside Neighbour if");
     }
     LOG_PRINT("Inside dv send for");
}
LOG_PRINT("Dv send done");

}
//Control Handler
#ifndef PACKET_USING_STRUCT
    #define CNTRL_CONTROL_CODE_OFFSET 0x04
    #define CNTRL_PAYLOAD_LEN_OFFSET 0x06
#endif
void log_print(char* filename, int line, char *fmt,...);
#define LOG_PRINT(...) log_print(__FILE__, __LINE__, __VA_ARGS__ )
/* Linked List for active control connections */
struct ControlConn
{
    int sockfd;
    LIST_ENTRY(ControlConn) next;
}*connection, *conn_temp;
LIST_HEAD(ControlConnsHead, ControlConn) control_conn_list;
// struct DataConn
// {
//     int sockfd;
//     LIST_ENTRY(DataConn) next;
// }*connection, *conn_temp;
// LIST_HEAD(DataConnsHead, DataConn) data_conn_list;


int create_control_sock()
{
    int sock;
    struct sockaddr_in control_addr;
    socklen_t addrlen = sizeof(control_addr);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
        ERROR("socket() failed");
    int a[] = {1};
    /* Make socket re-usable */
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, a, sizeof(int)) < 0)
        ERROR("setsockopt() failed");

    bzero(&control_addr, sizeof(control_addr));

    control_addr.sin_family = AF_INET;
    control_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    control_addr.sin_port = htons(CONTROL_PORT);

    if(bind(sock, (struct sockaddr *)&control_addr, sizeof(control_addr)) < 0)
        ERROR("bind() failed");

    if(listen(sock, 5) < 0)
        ERROR("listen() failed");

    LIST_INIT(&control_conn_list);

    return sock;
}

int new_control_conn(int sock_index)
{
    int fdaccept; socklen_t caddr_len;
    struct sockaddr_in remote_controller_addr;

    caddr_len = sizeof(remote_controller_addr);
    fdaccept = accept(sock_index, (struct sockaddr *)&remote_controller_addr, &caddr_len);
    if(fdaccept < 0)
        ERROR("accept() failed");

    /* Insert into list of active control connections */
    connection = (struct ControlConn *)malloc(sizeof(struct ControlConn));
    connection->sockfd = fdaccept;
    LIST_INSERT_HEAD(&control_conn_list, connection, next);

    return fdaccept;
}

void remove_control_conn(int sock_index)
{
    LIST_FOREACH(connection, &control_conn_list, next) {
        if(connection->sockfd == sock_index) LIST_REMOVE(connection, next); // this may be unsafe?
        free(connection);
    }

    close(sock_index);
}

bl isControl(int sock_index)
{
    LIST_FOREACH(connection, &control_conn_list, next)
        if(connection->sockfd == sock_index) return TRUE;

    return FALSE;
}

bl control_recv_hook(int sock_index)
{
     printf("I am here 4");
    char *cntrl_header, *cntrl_payload;
    uint8_t control_code;
    uint16_t payload_len;
    // int r_socket;
    /* Get control header */
    cntrl_header = (char *) malloc(sizeof(char)*CNTRL_HEADER_SIZE);
    bzero(cntrl_header, CNTRL_HEADER_SIZE);

    if(recvALL(sock_index, cntrl_header, CNTRL_HEADER_SIZE) < 0){
        remove_control_conn(sock_index);
        free(cntrl_header);
        return FALSE;
    }

    /* Get control code and payload length from the header */
    #ifdef PACKET_USING_STRUCT
        /** ASSERT(sizeof(struct CONTROL_HEADER) == 8) 
          * This is not really necessary with the __packed__ directive supplied during declaration (see control_header_lib.h).
          * If this fails, comment #define PACKET_USING_STRUCT in control_header_lib.h
          */
        BUILD_BUG_ON(sizeof(struct CONTROL_HEADER) != CNTRL_HEADER_SIZE); // This will FAIL during compilation itself; See comment above.

        struct CONTROL_HEADER *header = (struct CONTROL_HEADER *) cntrl_header;
        control_code = header->control_code;
        payload_len = ntohs(header->payload_len);
    #endif
    #ifndef PACKET_USING_STRUCT
        memcpy(&control_code, cntrl_header+CNTRL_CONTROL_CODE_OFFSET, sizeof(control_code));
        memcpy(&payload_len, cntrl_header+CNTRL_PAYLOAD_LEN_OFFSET, sizeof(payload_len));
        payload_len = ntohs(payload_len);
    #endif
    free(cntrl_header);
    cout<<payload_len<<"\n";
    /* Get control payload */
    if(payload_len != 0){
        cntrl_payload = (char *) malloc(sizeof(char)*payload_len);
        bzero(cntrl_payload, payload_len);
         cout<<payload_len<<"\n";
         cout<<cntrl_payload<<"\n";
        if(recvALL(sock_index, cntrl_payload, payload_len) < 0){
            remove_control_conn(sock_index);
            free(cntrl_payload);
            return FALSE;
        }
    }
    // std::cout<<cntrl_payload;

    /* Triage on control_code */
    switch(control_code){
        case 0: author_response(sock_index);
                printf("I am here author\n");
                 LOG_PRINT("I am in Author\n");
                break;

        case 1: printf("I am here init\n");
            
                LOG_PRINT("I am inside init case\n");
                init_payload(cntrl_payload);
                 LOG_PRINT("I am after Init Payload\n");
                router_socket = create_router_sock();
                FD_SET(router_socket, &master_list);
        if(router_socket > head_fd) {
            head_fd = router_socket;}
                data_socket = create_data_sock();
                FD_SET(data_socket, &master_list);
        if(data_socket > head_fd) {
            head_fd = router_socket;}
            printf("\nI am here response\n");
            init_response(sock_index);
             LOG_PRINT("I am after Init\n");
                break;
        case 2: 
        // if(init_rt)
         LOG_PRINT("I am in rt\n");
                std::cout<<"I am here rt\n";
                rt_response(sock_index);
          LOG_PRINT("I am after rt\n");
                break;
 /*
            .........
           ....... 
         ......*/
    }
    if(payload_len != 0) free(cntrl_payload);
    return TRUE;
}
//Connection Manager


void main_loop()
{
    int selret, sock_index, fdaccept;
    struct timeval time_val;
    time_val.tv_sec = TIME_PERIOD;
    time_val.tv_usec = 0;
    while(TRUE){
        watch_list = master_list;
        if(TIME_PERIOD != 0)
        selret = select(head_fd+1, &watch_list, NULL, NULL, &time_val);
        else
        {
            selret = select(head_fd+1, &watch_list, NULL, NULL, NULL);
        }
        
        if(selret < 0)
        {    ERROR("select failed.");}
        else
        {
            if(selret == 0){
            create_dv_payload();
             dv_send();}
        }
        
        
        /* Loop through file descriptors to check which ones are ready */
        for(sock_index=0; sock_index<=head_fd; sock_index+=1){
            printf("I am here 1");
            if(FD_ISSET(sock_index, &watch_list)){

                /* control_socket */
                if(sock_index == control_socket){
                    fdaccept = new_control_conn(sock_index);
                     printf("I am here 2");
                    /* Add to watched socket list */
                    FD_SET(fdaccept, &master_list);
                    if(fdaccept > head_fd) head_fd = fdaccept;
                }
               
                /* router_socket */
                else if(sock_index == router_socket){
                    //call handler that will call recvfrom() .....
                }

                /* data_socket */
                else if(sock_index == data_socket){
                    //new_data_conn(sock_index);
                }

                /* Existing connection */
                else{
                    if(isControl(sock_index)){
                         printf("I am here 3");
                        if(!control_recv_hook(sock_index)) FD_CLR(sock_index, &master_list);
                    }
                    //else if isData(sock_index);
                    else ERROR("Unknown socket index");
                }
            }
        }
    }
}

void init()
{
    control_socket = create_control_sock();

    //router_socket and data_socket will be initialized after INIT from controller

    FD_ZERO(&master_list);
    FD_ZERO(&watch_list);

    /* Register the control socket */
    FD_SET(control_socket, &master_list);
    head_fd = control_socket;

    main_loop();
}

int main(int argc, char **argv)
{
	/*Start Here*/
    logname += std::to_string(getpid());
    sscanf(argv[1], "%" SCNu16, &CONTROL_PORT);
    LOG_PRINT("I am in Main\n");
    init(); // Initialize connection manager; This will block
	
	return 0;
}
