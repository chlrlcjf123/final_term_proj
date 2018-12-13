#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 31842

struct Rule_Protocol_Header{
    unsigned char Protocol;
    unsigned char Data_len[3];
};

int main(void){
    int server_sock = -1;
    int client_sock = -1;
    int client_addr_size = -1;
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    struct Rule_Protocol_Header rule_hdr = {0};

    if((server_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1){
        perror("[Error] socket() error: ");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        perror("[Error] bind() error: ");
        exit(1);
    }

    if(listen(server_sock, 5) == -1){
        perror("[Error] listen() error: ");
        exit(1);
    }

    client_addr_size = sizeof(client_addr);
    if((client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size)) == -1){
        perror("[Error] accept() error: ");
        exit(1);
    }


}

