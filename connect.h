#ifndef CONNECT_H
#define CONNECT_H

int connectForSend(void);
int connectForGet(const char *ipaddr);
void closeConnect(int socket);


#endif

