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
using namespace std;
// #define AUTHOR_STATEMENT "I, gursimr2, have read and understood the course academic integrity policy."
void rt_response(int sock_index)
{
	uint16_t payload_len, response_len;
	char *cntrl_response_header, *cntrl_response_payload, *cntrl_response;

    cntrl_response_payload = rt_payload(sock_index);
	payload_len = sizeof(cntrl_response_payload); // Discount the NULL chararcter
	// cntrl_response_payload = (char *) malloc(payload_len);
	// memcpy(cntrl_response_payload, 0, payload_len);

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
 char* payload;
 uint16_t padding=0;
std::map <int,int>:: iterator it1= router_table.begin();
std::map <int,int>:: iterator it2 = next_hop.begin();
while(it1!=router_table.end() && it2!=next_hop.end()){
payload+=htons(it1->first);
payload+=htons(padding);
payload+=htons(it2->second);
payload+=htons(it1->second);
}
return payload;
}

//Create file for Routing Table response