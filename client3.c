#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>       //random function

// error function
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

// socket protocol struct 
typedef struct _socket_msg
{
	int magic;
	int seq_id;
	int trans_id;
	int type;
	int len;
	char data[64];
}message_t;

//main entry
int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    message_t msg_protocol, msg_protocol_read;

    memset(&msg_protocol, 0x00, sizeof(msg_protocol));

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    portno = atoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd < 0) 
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);

    if (server == NULL) 
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;

    bcopy((char *)server->h_addr, \
         (char *)&serv_addr.sin_addr.s_addr, \
         server->h_length);

    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    { 
        error("ERROR connecting");
    }

    srand(time(NULL));

    msg_protocol.magic = 0x12345678;
    msg_protocol.seq_id = 0x00;
    
    msg_protocol.type = 0x03;

    static int seq_id = 0x00;

    char szTemp[1024] = {0x00, };
    char* psz = szTemp;

    while (1)
    {
    	//printf("Please enter the message: ");
    	bzero(buffer,256);
    	memset(msg_protocol.data, 0x00, sizeof(msg_protocol));

	    printf("enter send data: ");
    	
        fgets(msg_protocol.data, sizeof(msg_protocol.data), stdin);
	    msg_protocol.len = strlen(msg_protocol.data);
        msg_protocol.seq_id = seq_id;
        msg_protocol.trans_id = rand();

    	n = write(sockfd, &msg_protocol, sizeof(msg_protocol));

        seq_id++;

    	if (n < 0) 
        {
         	error("ERROR writing to socket");
        }
    
   	    bzero(&msg_protocol_read,sizeof(msg_protocol_read));
    
    	n = read(sockfd, &msg_protocol_read, sizeof(msg_protocol_read));

  	    if (n < 0)
        {
         	error("ERROR reading from socket");
        }

    	if (msg_protocol_read.seq_id != msg_protocol.seq_id)
        {
            printf("ERROR : failed to match id\r\n");
        }

        if (msg_protocol_read.trans_id != msg_protocol.trans_id  )
        {
            printf("ERROR : failed to match trans id \r\n");
        }

        sprintf(psz, "[Info] maginc number : %08X \r\n", msg_protocol_read.magic);
        psz += strlen(psz);
        sprintf(psz, "[Info] sequence id : %d \r\n", msg_protocol_read.seq_id);
        psz += strlen(psz);
        sprintf(psz, "[Info] trans id : %d \r\n", msg_protocol_read.trans_id);
        psz += strlen(psz);
        sprintf(psz, "[Info] type : %d \r\n", msg_protocol_read.type);
        psz += strlen(psz);
        sprintf(psz, "[Info] data len : %d \r\n", msg_protocol_read.len);
        psz += strlen(psz);
        sprintf(psz, "[Info] payolad data : %s \r\n", msg_protocol_read.data);
        psz += strlen(psz);

        printf("Total data : \r\n %s \r\n", szTemp);
    	printf("%s", msg_protocol_read.data);
        printf("==================================================\r\n");
    }

    close(sockfd);
    return 0;
}
