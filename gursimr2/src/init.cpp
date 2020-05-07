/**
 * @author
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
 * INIT [Control Code: 0x01]
 */

#include "../include/global.h"
#include "../include/network_util.h"
#include "../include/control_header_lib.h"
#include "../include/author.h"
#include "../include/init.h"
#include "../include/routertable.h"
#include "../include/connection_manager.h"
#include <string.h>
#include <arpa/inet.h>
#include <vector>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
using namespace std;
void log_print(char* filename, int line, char *fmt,...);
#define LOG_PRINT(...) log_print(__FILE__, __LINE__, __VA_ARGS__ )
// void log_print(char* filename, int line, char *fmt,...);
// #define LOG_PRINT(...) log_print(__FILE__, __LINE__, __VA_ARGS__ )

// /* logger.c */
// #include <stdio.h>
// #include <stdarg.h>
// #include <time.h>
// #include <string.h>
// #include <stdlib.h>
// #include <iostream>
// #include <string>
// #include <unistd.h>
// #include "../include/logger.h"
// FILE *fp ;
// static int SESSION_TRACKER; //Keeps track of session
// std::string logname = "/tmp/gursimr2init.txt";

// char* print_time()
// {
//     int size = 0;
//     time_t t;
//     char *buf;
    
//     t=time(NULL); /* get current calendar time */
    
//     char *timestr = asctime( localtime(&t) );
//     timestr[strlen(timestr) - 1] = 0;  //Getting rid of \n
    
//     size = strlen(timestr)+ 1 + 2; //Additional +2 for square braces
//     buf = (char*)malloc(size);
    
//     memset(buf, 0x0, size);
//     snprintf(buf,size,"[%s]", timestr);
   
//     return buf;
// }

// void log_print(char* filename, int line, char *fmt,...)
// {
//     va_list list;
//     char *p, *r;
//     int e;

    
//     if(SESSION_TRACKER > 0)
//       fp = fopen (logname.c_str(),"a+");
//     else {
//       fp = fopen (logname.c_str(),"w");
//     }
//     fprintf(fp,"%s ",print_time());
//     fprintf(fp,"[%s][line: %d] ",filename,line);
//     va_start( list, fmt );

//     for ( p = fmt ; *p ; ++p )
//     {
//         if ( *p != '%' )//If simple string
//         {
//             fputc( *p,fp );
//         }
//         else
//         {
//             switch ( *++p )
//             {
//                 /* string */
//             case 's':
//             {
//                 r = va_arg( list, char * );

//                 fprintf(fp,"%s", r);
//                 continue;
//             }

//             /* integer */
//             case 'd':
//             {
//                 e = va_arg( list, int );

//                 fprintf(fp,"%d", e);
//                 continue;
//             }

//             default:
//                 fputc( *p, fp );
//             }
//         }
//     }
//     va_end( list );
//     fputc( '\n', fp );
//     SESSION_TRACKER++;
//     fclose(fp);
// }
// struct DataConn
// {
//         int sockfd;
//         LIST_ENTRY(DataConn) next;
// }*connection, *conn_temp;
// LIST_HEAD(DataConnsHead, DataConn) data_conn_list;

// #define AUTHOR_STATEMENT "I, gursimr2, have read and understood the course academic integrity policy."
char HOSTIP[20];
uint16_t MYID;
uint16_t NUM_ROUTERS;
uint16_t TIME_PERIOD;
uint16_t ROUTER_PORT;
uint16_t DATA_PORT;
ROUTER_TABLE rt[MAX_ROUTERS];
#include "../include/logger.h"
// bool init_rt=false;
void init_response(int sock_index)
{
	uint16_t payload_len=0, response_len=0;
	char *cntrl_response_header, *cntrl_response_payload, *cntrl_response;
	// LOG_PRINT("I am inside init response\n");
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
	// LOG_PRINT("%d\n",response_len);
	// LOG_PRINT("Init Response Sent\n");
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
			rt[i].router_id = ntohs(rti->router_id);
			rt[i].cost=ntohs(rti->cost);
			rt[i].next_hop=ntohs(rti->router_id);
			rt[i].neighbour = FALSE;
		}else{
			if(ntohs(rti->cost) == 65535){
				rt[i].router_id = ntohs(rti->router_id);
				rt[i].cost=ntohs(rti->cost);
				rt[i].next_hop=ntohs(rti->cost);
				rt[i].neighbour = FALSE;

			}else{
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
  }
// LOG_PRINT("exit init payload\n");
//   cout<<r[0].router_id<<"\n";
//   cout<<r[0].r_port<<"\n";
//   char* temp;
//   char* ip;
//   std::cout<<cntrl_payload<<"\n";
//   memcpy(temp,cntrl_payload,sizeof(uint16_t));
//   sscanf(temp,"%d",num_r);
// //   num_r = (uint16_t)temp;
// 	std::cout<<cntrl_payload<<"\n";
// 	std::cout<<temp<<"\n";
// 	std::cout<<num_r<<"\n";
//   NUM_ROUTERS = ntohs(num_r);
// 	std::cout<<NUM_ROUTERS<<"\n";
//   printf("%d",NUM_ROUTERS);
//   memcpy(temp,cntrl_payload +  sizeof(uint16_t),sizeof( uint16_t));
// //   time_p =  (uint16_t)temp;
//   sscanf(temp,"%d",time_p);
//   TIME_PERIOD = ntohs(time_p);
//   for(int i=0;i<num_r;i++){
// 	  memcpy(temp,cntrl_payload +  (i*6*sizeof(uint16_t)) +2*sizeof(uint16_t),sizeof( uint16_t));
// 	  sscanf(temp,"%d",r[i].router_id);
// 	//   r[i].router_id=(uint16_t)temp;
// 	  r[i].router_id=ntohs(r[i].router_id);
// 	  memcpy(temp,cntrl_payload +  (i*6*sizeof(uint16_t)) +3*sizeof(uint16_t),sizeof( uint16_t));
// 	  sscanf(temp,"%d",r[i].r_port);
// 	  //r[i].r_port=(uint16_t)temp;
// 	  r[i].r_port=ntohs(r[i].r_port);
// 	  memcpy(temp,cntrl_payload +  (i*6*sizeof(uint16_t)) +4*sizeof(uint16_t),sizeof( uint16_t));
// 	  sscanf(temp,"%d",r[i].d_port);
// 	  //r[i].d_port=(uint16_t)temp;
// 	  r[i].d_port=ntohs(r[i].d_port);
// 	  memcpy(temp,cntrl_payload +  (i*6*sizeof(uint16_t)) +5*sizeof(uint16_t),sizeof( uint16_t));
// 	  sscanf(temp,"%d",r[i].cost);
// 	  //r[i].cost=(uint16_t)temp;
// 	  r[i].cost=ntohs(r[i].cost);
// 	  memcpy(temp,cntrl_payload +  (i*6*sizeof(uint16_t)) +6*sizeof(uint16_t),2*sizeof( uint16_t));
// 	  inet_ntop(AF_INET,temp,ip,sizeof(INET_ADDRSTRLEN));
// 	  strncpy(r[i].ip,ip,sizeof(r[i].ip));

// 		if(r[i].cost == 0){
// 			MYID=r[i].router_id;
// 			ROUTER_PORT=r[i].r_port;
// 			DATA_PORT = r[i].d_port;
// 			rt[i].router_id = r[i].router_id;
// 			rt[i].cost=r[i].cost;
// 			rt[i].next_hop=r[i].router_id;
// 			rt[i].neighbour = false;
// 		}else{
// 			if(r[i].cost == 65535){
// 				rt[i].router_id = r[i].router_id;
// 				rt[i].cost=r[i].cost;
// 				rt[i].next_hop=r[i].cost;
// 				rt[i].neighbour = false;
// 			}
// 			else{
// 				rt[i].router_id=r[i].router_id;
// 				rt[i].cost=r[i].cost;
// 				rt[i].next_hop=r[i].router_id;
// 				rt[i].neighbour = true;
// 			}
// 		}
	
//   }
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

//Create file for Routing Table response