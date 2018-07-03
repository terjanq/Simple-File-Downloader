#ifndef SOCK_H_
#define SOCK_H_

#include "utils.h"


typedef struct my_socket{
    int 				sockfd;
    struct sockaddr_in 	server_address;
    struct sockaddr_in 	sender;	
    socklen_t           sender_len;
    char           		msg_buffer[ IP_MAXPACKET + 1 ];
    int 				msg_len;
    int                 msg_offset;
} my_socket;




my_socket initSocket( char *IP, int PORT );

int sendPacket( char *msg, int msg_len, my_socket *MS );
int receivePacket( my_socket *MS );

#endif
