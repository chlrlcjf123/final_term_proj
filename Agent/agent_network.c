#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#include "../common_header/common.h"

#define  BUFF_SIZE   1024

typedef struct {
    long data_type;
    int sub_type;
    char data_buff[BUFF_SIZE];
} t_data;

int main(void){
    int server_sock = -1;
    int client_sock = -1;
    int client_addr_size = -1;
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    struct Rule_Protocol_Header *rule_hdr = {0};

    int ret_len = 0;
    unsigned char buff_recv[4] = {0};
    unsigned char buff_data[100] = {0};

    int msqid;
    t_data data;

    if ( -1 == ( msqid = msgget((key_t)3856, IPC_CREAT | 0666))){
        perror("[Error] msgget() 실패 :");
        exit(1);
    }

    /* socket Create */
    if((server_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1){
        perror("[Error] socket() error: ");
        exit(1);
    }

    /* Server Address Conf */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* Bind() function */
    if(bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        perror("[Error] bind() error: ");
        exit(1);
    }

    /* listen() function */
    if(listen(server_sock, 5) == -1){
        perror("[Error] listen() error: ");
        exit(1);
    }

    /* accept() function */
    client_addr_size = sizeof(client_addr);
    if((client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size)) == -1){
        perror("[Error] accept() error: ");
        exit(1);
    }

    while(1){
        rule_hdr = NULL;

        ret_len = recv(client_sock, buff_recv, sizeof(buff_recv), 0);

        if(ret_len>0){
            rule_hdr = (struct Rule_Protocol_Header *)buff_recv;

            if(rule_hdr->Protocol & 1){
                printf("[Info] Firewall Rule Add\n");
                data.data_type = 1;
                data.sub_type = 1;
            }else if(rule_hdr->Protocol & 2){
                printf("[Info] Process Rule Add\n");
                data.data_type = 2;
                data.sub_type = 1;
            }else if(rule_hdr->Protocol & 4){
                printf("[Info] Firewall Rule Delete\n");
                data.data_type = 1;
                data.sub_type = 2;
            }else if(rule_hdr->Protocol & 8){
                printf("[Info] Process Rule Delete\n");
                data.data_type = 2;
                data.sub_type = 2;
            }else{
                printf("[Error] The Wrong Approach!\n");
            }

            ret_len = recv(client_sock, buff_data ,(size_t)&rule_hdr->Data_len, 0);
            sprintf( data.data_buff, "%s", buff_data);
            printf("recv len : %d data : %s\n", ret_len, buff_data);

            if ( -1 == msgsnd(msqid, &data, sizeof(t_data) - sizeof(long), 0)){
                perror("[Error] msgsnd() 실패");
                exit(1);
            }

            sleep(1);
        }
    }
}