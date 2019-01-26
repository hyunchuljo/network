#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"
//소켓 프로그래밍에 사용될 헤더파일 선언
 
#define BUF_LEN 128
//메시지 송수신에 사용될 버퍼 크기를 선언

#define socket_send	(1)
#define MAX_DATA_SIZE   (64)

typedef struct _common_protocol
{
    int _imagic;
    int _iseq_id;
    int _itrans_id;
    int _itype;
    char _chData[MAX_DATA_SIZE];
} COMMON;

int check_validation(void* input);
int g_seq_id = 0x00;

int main(int argc, char *argv[])
{
    COMMON buffer;
    struct sockaddr_in server_addr, client_addr;
    char temp[20];
    int server_fd, client_fd;
    //server_fd, client_fd : 각 소켓 번호
    int len, msg_size;
 
    if(argc != 2)
    {
        printf("usage : %s [port]\n", argv[0]);
        exit(0);
    }
 
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {// 소켓 생성
        printf("Server : Can't open stream socket\n");
        exit(0);
    }
    memset(&server_addr, 0x00, sizeof(server_addr));
    //server_Addr 을 NULL로 초기화
 
    server_addr.sin_family = AF_INET;
    //SERVER RECEIVE ALL DATA, 
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));
    //server_addr 셋팅

    //charles : socket reuse option added....
    int enable = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
	    error("## failed to (SO_REUSEADDR) failed");

    if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <0)
    {//bind() 호출
        printf("Server : Can't bind local address.\n");
        exit(0);
    }
 
    if(listen(server_fd, 5) < 0)
    {//소켓을 수동 대기모드로 설정
        printf("Server : Can't listening connect.\n");
        exit(0);
    }
 
    memset(buffer, 0x00, sizeof(buffer));
    printf("Server : wating connection request.\n");
    len = sizeof(client_addr);

    while(1)
    {
        // @charles : accetp to request connection!
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &len);

	    printf("## Entry after function called accept API \r\n"); 

        if(client_fd < 0)
        {
            printf("Server: accept failed.\n");
            close(server_fd);   //? close server socket before program exit.
            exit(0);
        }

        inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, temp, sizeof(temp));
        printf("Server : %s client connected.\n", temp);
    
        while (1)
        {
            msg_size = read(client_fd, buffer, 1024);

            if (msg_size < 0)
            {
                printf("failed to read API \r\n");
                break;
            }

            if (!strncmp(buffer._chData, "quit", 4))
            {
                printf("good bye my clinet \r\n");
                break;
            }

            char    temp_buf[128] = {0,};
            char*   szTemp = temp_buf;
            sprintf(szTemp, "server res: ", sizeof(temp_buf));
            szTemp += strlen(szTemp);

    #if (socket_send == 0)
            write(client_fd, temp_buf, sizeof(temp_buf));
            //write(client_fd, buffer, msg_size);
    #else
            send(client_fd, temp_buf, sizeof(temp_buf), 0);
            //send(client_fd, buffer, msg_size, 0);

            sprintf(szTemp, "magic ID: %d \r\n", buffer._imagic);
            szTemp += strlen(szTemp);
            sprintf(szTemp, "seq_id : %d \r\n", buffer._iseq_id);
            szTemp += strlen(szTemp);
            sprintf(szTemp, "trans_id: %d \r\n", g_seq_id -1);
            szTemp += strlen(szTemp);
            sprintf(szTemp, "type : 1 \r\n");
            szTemp += strlen(szTemp);
            sprintf(szTemp, "Data : %s \r\n", buffer._chData);
            szTemp += strlen(szTemp);

            send(client_fd, temp_buf, msg_size, 0);

    #endif
        }
        close(client_fd);
        printf("Server : %s client closed.\n", temp);
    }

    close(server_fd);

    return 0;
}

int check_validation(void* input)
{
    COMMON* szCommon = (COMMON*)input;

    if (g_seq_id != szCommon->_iseq_id)
    {
        return 0x01;
    }
    g_seq_id++;
}