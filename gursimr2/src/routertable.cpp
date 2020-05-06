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

    char* payload= rt_payload(sock_index);
	std::cout<<"I am here rt-payload in response- "<<payload<<"\n";
	payload_len = sizeof(payload); // Discount the NULL chararcter
	cntrl_response_payload = (char *) malloc(payload_len);
	std::cout<<"I am here rt-payload length in response- "<<payload_len<<"\n";
	memcpy(cntrl_response_payload, payload , payload_len);

	cntrl_response_header = create_response_header(sock_index, 2, 0, payload_len);

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
char* rt_payload(int sock_index){
//save data from init control packet to routing table
 char* payload=new char[8*NUM_ROUTERS];
 char *buffer;
//  payload = (char *) malloc(sizeof(char)*);
//  payload = (char *) malloc(sizeof(char)*8);
 struct routing_tab_payload *rtresponse[NUM_ROUTERS];
 uint16_t padding=0;
 std::cout<<"I am here rt-payload\n";
 std::cout<<NUM_ROUTERS<<"\n";
 	std::cout<<rt[0].router_id<<"\n";
	 	std::cout<<rt[0].neighbour<<"\n";
for(int i=0;i<NUM_ROUTERS;i++){
	std::cout<<rt[i].router_id<<"\n";
	std::cout<<padding<<"\n";
	// rtresponse[i] = (struct routing_tab_payload *) (buffer);
rtresponse[i]->router_id=htons(rt[i].router_id);
rtresponse[i]->padding+=htons(padding);
rtresponse[i]->next_hop+=htons(rt[i].next_hop);
rtresponse[i]->cost+=htons(rt[i].cost);
// std::cout<<buffer<<"\n";
memcpy(payload + i*sizeof(routing_tab_payload), &rtresponse[i], sizeof(routing_tab_payload));
std::cout<<ntohs(rtresponse[i]->router_id)<<"\n";
}
std::cout<<"I am here rt-payload in response- "<<payload<<"\n";
return payload;
}

//Create file for Routing Table response