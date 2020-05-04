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

#include <string.h>
#include <arpa/inet.h>
#include <vector>
#include <string>
#include <netinet/in.h>
using namespace std;
// #define AUTHOR_STATEMENT "I, gursimr2, have read and understood the course academic integrity policy."

void init_response(int sock_index)
{
	uint16_t payload_len, response_len;
	char *cntrl_response_header, *cntrl_response_payload, *cntrl_response;

	payload_len = 0; // Discount the NULL chararcter
	// cntrl_response_payload = (char *) malloc(payload_len);
	// memcpy(cntrl_response_payload, 0, payload_len);

	cntrl_response_header = create_response_header(sock_index, 1, 0, payload_len);

	response_len = CNTRL_RESP_HEADER_SIZE+payload_len;
	cntrl_response = (char *) malloc(response_len);
	/* Copy Header */
	memcpy(cntrl_response, cntrl_response_header, CNTRL_RESP_HEADER_SIZE);
	free(cntrl_response_header);
	/* Copy Payload */
	// memcpy(cntrl_response+CNTRL_RESP_HEADER_SIZE, cntrl_response_payload, payload_len);
	// free(cntrl_response_payload);

	sendALL(sock_index, cntrl_response, response_len);

	free(cntrl_response);
}
void init_payload(int sock_index,char *cntrl_payload){
//save data from init control packet to routing table
vector<router_init> r;
 uint16_t num_r;
  uint16_t time_p;
  char* temp;
  char* ip;
  strncpy(temp,cntrl_payload,sizeof(uint16_t));
  num_r = (uint16_t)temp;
  NUM_ROUTERS = ntohs(num_r);
  strncpy(temp,cntrl_payload +  sizeof(uint16_t),sizeof( uint16_t));
  time_p =  (uint16_t)temp;
  TIME_PERIOD = ntohs(time_p);
  for(int i=0;i<num_r;i++){
	  strncpy(temp,cntrl_payload +  (i*6*sizeof(uint16_t)) +2*sizeof(uint16_t),sizeof( uint16_t));
	  r[i].router_id=(uint16_t)temp;
	  r[i].router_id=ntohs(r[i].router_id);
	  strncpy(temp,cntrl_payload +  (i*6*sizeof(uint16_t)) +3*sizeof(uint16_t),sizeof( uint16_t));
	  r[i].r_port=(uint16_t)temp;
	  r[i].r_port=ntohs(r[i].r_port);
	  strncpy(temp,cntrl_payload +  (i*6*sizeof(uint16_t)) +4*sizeof(uint16_t),sizeof( uint16_t));
	  r[i].d_port=(uint16_t)temp;
	  r[i].d_port=ntohs(r[i].d_port);
	  strncpy(temp,cntrl_payload +  (i*6*sizeof(uint16_t)) +5*sizeof(uint16_t),sizeof( uint16_t));
	  r[i].cost=(uint16_t)temp;
	  r[i].cost=ntohs(r[i].cost);
	  strncpy(temp,cntrl_payload +  (i*6*sizeof(uint16_t)) +6*sizeof(uint16_t),2*sizeof( uint16_t));
	  inet_ntop(AF_INET,temp,ip,sizeof(INET_ADDRSTRLEN));
	  strncpy(r[i].ip,ip,sizeof(r[i].ip));

		if(r[i].cost == 0){
			MYID=r[i].router_id;
			router_table[r[i].router_id]=r[i].cost;
			next_hop[r[i].router_id]=r[i].router_id;
		}else{
			if(r[i].cost == 65535){
				router_table[r[i].router_id]=r[i].cost;
				next_hop[r[i].router_id]=r[i].cost;
			}
			else{
				router_table[r[i].router_id]=r[i].cost;
				next_hop[r[i].router_id]=r[i].router_id;
			}
		}
	
  }
}

//Create file for Routing Table response