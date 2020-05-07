#ifndef INIT_H_
#define INIT_H_

void init_response(int sock_index);
void init_payload(char *cntrl_payload);
int create_router_sock();
int create_data_sock();
#endif