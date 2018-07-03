// Jan Góra (272472)
// Transporter, 14.05.2018

#include "utils.h"
#include "engine.h"
#include "sock.h"


#define IP_SERVER "156.17.4.30"


void parseMessage(my_socket *MS, int *start, int *dlugosc){

	char st[10], dl[10];
	char *s = st;
	char *d = dl;
	char *b = MS->msg_buffer + 5;

	while(*b != ' '){
		*s++ = *b++;
	}
	b++;
	while(*b != '\n'){
		*d++ = *b++;
	}

	*start = atoi(st);
	*dlugosc = atoi(dl);
	MS->msg_offset = MS->msg_len - *dlugosc;
}

int main(int argc, char* argv[])
{
	

	if (argc != 4) {
        fprintf (stderr, "usage: %s PORT FILE_NAME SIZE\n", argv[0]);
        return EXIT_FAILURE;
    }

    int PORT = atoi(argv[1]);

    if(PORT < 0 || PORT > 65535){
    	fprintf (stderr, "invalid port number\n");
        return EXIT_FAILURE;
    }

    FILE *output_file = fopen(argv[2], "wb");
    if ( output_file == NULL ){
    	fprintf (stderr, "failed to open %s\n", argv[2]);
        return EXIT_FAILURE;
    }
    file_info FI = initFile( atoi(argv[3]), &output_file);
    
	my_socket MS = initSocket( IP_SERVER, PORT );

	if ( MS.sockfd < 0 ) {
		fprintf(stderr, "socket error: %s\n", strerror(errno)); 
		return EXIT_FAILURE;
	}

	updateParts(&FI, &MS);

	fprintf(stdout, "Parts to download: %d\nDownloaded:       ", FI.parts_number );

	while(1){
		fprintf(stdout, "\b\b\b\b\b\b\b%*.2f%%", 6, 100.0*(1.0 - (float)FI.parts_left/FI.parts_number));
		MS.msg_len = receivePacket( &MS );
		if (MS.msg_len < 0) {
			fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
			return EXIT_FAILURE;
		}

		char sender_ip_str[20]; 
		inet_ntop(AF_INET, &(MS.sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));


		if( MS.msg_len > 0 && strcmp(IP_SERVER, sender_ip_str) == 0){
			int start=0, length=0;
			parseMessage(&MS, &start, &length);
			if (updateFileInfo(start, length, MS.msg_buffer + MS.msg_offset, &FI )) return 1;
			// printf("Received: %d %d\n", start, length);
		}

		updateParts(&FI, &MS);
		fflush(stdout);
	}

	fprintf(stdout, "\n");
	fflush(stdout);
	close (MS.sockfd);
	fclose(output_file);
	return EXIT_SUCCESS;
}