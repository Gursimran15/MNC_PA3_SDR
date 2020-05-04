#ifndef INIT_H_
#define INIT_H_

void init_response(int sock_index);
void init_payload(int sock_index,char *cntrl_payload);
int create_router_sock();
#endif