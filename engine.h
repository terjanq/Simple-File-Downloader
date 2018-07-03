#ifndef ENGINE_H_
#define ENGINE_H_

#include "utils.h"
#include "sock.h"


#define PACKET_LEN                  1000
#define PACKETS_PER_TURN            900
#define TMP_BUFFER_LEN              2000000
#define TERMINATE_TIME              1000 // miliseconds
#define TERMINATE_TIME2             800
#define WINDOW_SIZE                 1000000 // WINDOW_SIZE / PACKET_LEN
#define PACKETS_PER_REQUEST         4

typedef struct file_info{
    int                 size;
    int                 parts_number;
    int                 parts_left;
    int                 waiting_for;
    int                 part1_start;
    int                 parts_to_send;
    struct timeval*     parts_time;
    u_int8_t*           parts_done;
    char                TMP_BUFFER[TMP_BUFFER_LEN];
    FILE*               output_file;
} file_info;


file_info initFile( int size, FILE **output_file );


int updateFileInfo( int start, int length, char *bytes, file_info *FI );
void insertPart( int start, int length, char *bytes, file_info *FI );
void writeToBuffer( int start, int length, char *bytes, file_info *FI );
void writeToFile( file_info *FI );
void shiftBuffer( file_info *FI );
void updateParts( file_info *FI, my_socket *MS);
void trySendPart( int i, file_info *FI, my_socket *MS);
time_t timeToTerminate( struct timeval send_time );
ssize_t createHeader(int start, int dlugosc, char **header);

#endif