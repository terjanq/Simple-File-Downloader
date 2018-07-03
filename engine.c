// Jan Góra (272472)
// Transporter, 14.05.2018


#include "engine.h"

file_info initFile(int size, FILE **output_file){

    file_info FI;

    FI.size             = size;
    FI.parts_number     = (size + PACKET_LEN - 1) / PACKET_LEN;
    FI.parts_left       = FI.parts_number;
    FI.parts_done       = malloc(sizeof(u_int8_t)*FI.parts_number);
    FI.parts_time       = malloc(sizeof(struct timeval)*FI.parts_number);
    FI.waiting_for      = 0;
    FI.part1_start      = 0;
    FI.output_file      = *output_file;
    FI.parts_to_send    = PACKETS_PER_TURN;

    for(int i = 0; i < FI.parts_number; i++){
        FI.parts_done[i] = 0;
        FI.parts_time[i].tv_sec = INT_MAX;
        FI.parts_time[i].tv_usec = 0;
    }

    return FI;
}

int updateFileInfo( int start, int length, char *bytes, file_info *FI ){

    insertPart(start, length, bytes, FI);
    int doneTill = FI -> waiting_for * PACKET_LEN;

    if( doneTill >= FI -> size ){
        writeToFile( FI );
        return 1;
    }

    if( doneTill > FI->part1_start + WINDOW_SIZE ){
       writeToFile( FI );
       shiftBuffer( FI );
    }

    return 0;
}


void insertPart(int start, int length, char *bytes, file_info *FI){

    int fid = start / PACKET_LEN;
    FI->parts_to_send++;

    if(!FI->parts_done[fid]){
        FI->parts_done[fid] = 1;
        FI->parts_left--;
        writeToBuffer(start, length, bytes, FI);
        while( FI->parts_done[ FI->waiting_for ] ) FI->waiting_for++;
    }
}


void writeToBuffer(int start, int length, char *bytes, file_info *FI){
    int insert = start - FI->part1_start;
    memcpy( FI->TMP_BUFFER + insert, bytes, length );
}


void writeToFile(file_info *FI){
    size_t len = (FI -> waiting_for < FI -> parts_number  ? WINDOW_SIZE : FI -> size - FI -> part1_start);
    if( len != fwrite( FI -> TMP_BUFFER, sizeof FI->TMP_BUFFER[0], len, FI -> output_file ) ){
        fprintf(stderr, "fwrite error: %s\n", strerror(errno)); 
    }
}

void shiftBuffer(file_info *FI){

    for(int i = 0; i < TMP_BUFFER_LEN - WINDOW_SIZE; i++)
        FI->TMP_BUFFER[i] = FI->TMP_BUFFER[WINDOW_SIZE + i];
    FI -> part1_start += WINDOW_SIZE;
}


void updateParts( file_info *FI,  my_socket *MS ){
    int end = ( FI->part1_start + TMP_BUFFER_LEN ) / PACKET_LEN;

    for( int i = FI->waiting_for; i < FI->parts_number && i < end; i++ ){

        if( !FI->parts_done[i] ){

            if( FI -> parts_time[i].tv_sec == INT_MAX ) {
                trySendPart( i, FI, MS );
                trySendPart( i, FI, MS );
            }
            else{
                if(  timeToTerminate ( FI -> parts_time[i] ) <= 0 ){

                    FI -> parts_to_send += 1;
                    trySendPart( i, FI, MS );
                }
            }
        }
    }

    for( int i = FI->waiting_for; i < FI->parts_number && i < end && FI->parts_to_send > 0; i++ ){
        if( !FI->parts_done[i] && timeToTerminate( FI -> parts_time[i] ) <= TERMINATE_TIME2 ){
            trySendPart( i, FI, MS);
            trySendPart( i, FI, MS);

        }
    }
}


time_t timeToTerminate( struct timeval send_time ){
    struct timeval now;
    gettimeofday(&now, NULL);
    return TERMINATE_TIME - ( (now.tv_sec - send_time.tv_sec)*1000 + (now.tv_usec - send_time.tv_usec)/1000);
}

void trySendPart( int i, file_info *FI, my_socket *MS){
    if( FI->parts_to_send <= 0 ) return;
    // sendPacket()
    int p_len = ( i == FI->parts_number - 1? FI->size - i*PACKET_LEN : PACKET_LEN );
    char *msg;
    int msg_len = createHeader( i * PACKET_LEN, p_len, &msg );
    if( !sendPacket( msg, msg_len, MS ) ){
        fprintf(stderr, "sendto error: %s\n", strerror(errno)); 
    }
    free(msg);
    FI -> parts_to_send--;
    gettimeofday( &(FI -> parts_time[i]), NULL);
}


ssize_t createHeader(int start, int dlugosc, char **header){
    char *buf = malloc(23);
    sprintf(buf, "GET %d %d\n", start, dlugosc);
    *header = buf;

    return strlen(buf);
}