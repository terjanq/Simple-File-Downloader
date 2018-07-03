// Jan Góra (272472)
// Transporter, 14.05.2018


#include "sock.h"

my_socket initSocket(char *IP,  int PORT ){

    my_socket MS;

    MS.sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero ( &(MS.server_address), sizeof(MS.server_address) );
    MS.server_address.sin_family = AF_INET;
    MS.server_address.sin_port   = htons(PORT);
    inet_pton(AF_INET, IP, &(MS.server_address.sin_addr));

    return MS;
}


int sendPacket( char *msg, int msg_len, my_socket *MS ){
    // printf("Sending: %s\n", msg);
    return 
        sendto(MS->sockfd, msg, msg_len, 0, (struct sockaddr*) &(MS->server_address), sizeof(MS->server_address)) == msg_len;
}



int receivePacket( my_socket *MS ){
    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(MS->sockfd, &descriptors);
    struct timeval tv = {0, 10000};
    int ready = select(MS->sockfd + 1, &descriptors, NULL, NULL, &tv);
    if (ready <= 0) return ready;
    return (int)( recvfrom (MS->sockfd, MS->msg_buffer, IP_MAXPACKET, 0, (struct sockaddr*)&(MS->sender), &(MS->sender_len)) );
}

        

        // // FILE *f = fopen("")
        // int start, dlugosc;

        // parseMessage(&buffer, &start, &dlugosc);
