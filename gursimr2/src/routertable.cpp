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
#include "../include/control_header_lib.h"
#include "../include/network_util.h"
#include "../include/routertable.h"
#include <string.h>
#include <arpa/inet.h>
#include <vector>
#include <iterator>
#include <string>
#include <netinet/in.h>
#include <iostream>
using namespace std;
// //ERROR IN THIS FILE
// uint16_t NUM_ROUTERS;
// ROUTER_TABLE rt[MAX_ROUTERS];
// // #define AUTHOR_STATEMENT "I, gursimr2, have read and understood the course academic integrity policy."
void rt_response(int sock_index)
{
	std::cout<<"I am here rt-response\n";
	uint16_t payload_len, response_len;
	char *cntrl_response_header, *cntrl_response_payload, *cntrl_response;
	uint16_t padding=0,tmp=0,o=0,num=NUM_ROUTERS;
    // char* payload= rt_payload(sock_index);
	//std::cout<<"I am here rt-payload in response- "<<payload<<"\n";
	payload_len = (sizeof(uint16_t))*4*5;// Discount the NULL chararcter
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
// char* rt_payload(int sock_index){
// //save data from init control packet to routing table
//  char* payload;
//  uint16_t payload_len = (sizeof(uint16_t))*4*5;
//  char *buffer;
//  payload = (char *) malloc(payload_len);
// 	bzero(payload,payload_len);
// //  payload = (char *) malloc(sizeof(char)*);
// //  payload = (char *) malloc(sizeof(char)*8);
//  routing_tab_payload rtresponse;
//  uint16_t padding=0,tmp=0,o=0;
//  std::cout<<"I am here rt-payload\n";
//  std::cout<<NUM_ROUTERS<<"\n";
//  	// std::cout<<rt[0].router_id<<"\n";
// 	//  	std::cout<<rt[0].neighbour<<"\n";
// for(int i=0;i<NUM_ROUTERS;i++){
// 	// std::cout<<rt[i].router_id<<"\n";
// 	// std::cout<<padding<<"\n";
// 	// rtresponse[i] = (struct routing_tab_payload *) (buffer);
// 	cout<<rt[i].router_id<<"\n";
// 		cout<<rt[i].cost<<"\n";
// 		cout<<rt[i].next_hop<<"\n";
// 		cout<<rt[i].neighbour<<"\n";
// tmp=htons(rt[i].router_id);
// //tmp=rtresponse.router_id;
// memcpy(payload + o,&tmp,2);
// o+=2;
// cout<<ntohs(tmp)<<"\n";
// tmp=htons(padding);
// //tmp=rtresponse.padding;
// memcpy(payload + o,&tmp,2);
// o+=2;
// cout<<ntohs(tmp)<<"\n";
// tmp=htons(rt[i].next_hop);
// //tmp=rtresponse.next_hop;
// memcpy(payload + o,&tmp,2);
// o+=2;
// cout<<ntohs(tmp)<<"\n";
// tmp=htons(rt[i].cost);
// //tmp=rtresponse.cost;
// memcpy(payload + o,&tmp,2);
// o+=2;
// cout<<ntohs(tmp)<<"\n";
// // cout<<rtresponse.router_id<<"\n";
// // 		cout<<rtresponse.cost<<"\n";
// // 		cout<<rtresponse.next_hop<<"\n";
// // 		cout<<rtresponse.padding<<"\n";
// 	// cout<<payload<<"\n";
// // std::cout<<buffer<<"\n";
// //cout<<sizeof(rtresponse[i])<<"\n";
// // cout<<sizeof(rtresponse)<<"\n";
// // cout<<sizeof(struct routing_tab_payload *)<<"\n";

// // memcpy(payload + i*sizeof(rtresponse[i]), rtresponse[i], sizeof(rtresponse[i])); //error here
// // std::cout<<ntohs(rtresponse[i]->router_id)<<"\n";
// }
// std::cout<<"I am here rt-payload in response- "<<payload<<"\n";
// return payload;
// }

//Create file for Routing Table response