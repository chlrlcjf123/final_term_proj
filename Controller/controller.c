#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../common_header/common.h"

int main(void){
    /* Socket Var */
    int sock=-1;
    struct sockaddr_in serv_addr;

    unsigned char send_buff[4];

    /* Sock Create */
    if( (sock = socket(PF_INET, SOCK_STREAM, 0)) == -1){
        perror("[Error] socket() error : ");
        exit(1);
    }

    /* Server Address Conf */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERV_ADDR);
    serv_addr.sin_port = htons(PORT);

    /* Sock Connect */
    if( (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) == -1){
        perror("[Error] connect() error : ");
        exit(1);
    }

    /* Send Rule Loop */
    while(1){
        struct Rule_Protocol_Header rule_hdr = {0};
        char rule_data[1024] = {0};

        memset(rule_data, 0, sizeof(rule_data));

        printf("Input Rule Protocol Number (ex. 1: IP/Port Rule Add, 2: Process Rule Add, 4: IP/Port Rule Delete, 8: Process Rule Delete) \n: ");
        scanf(" %c", &rule_hdr.Protocol);

        printf("Input Rule Data \n IP/Port Rule Add => [IP;Port]\n IP/Port Rule Delete => Input Rule Number\n Process Rule Add/Delete => Process Name\n : ");
        scanf("%s", rule_data);

        /* Data len convert : (int) -> (hex) */
        sprintf(rule_hdr.Data_len, "%02x", (int)strlen(rule_data));

        /* Set Rule_Protocol_Header */
        /* (int)rule_hdr.Protocol-48 : ascii code -> int */
        sprintf(send_buff, "%d%s", (int)rule_hdr.Protocol-48, rule_hdr.Data_len);
        printf("%s\n",send_buff);

        /* Send Rule_Protocol_Header */
        if(send(sock, send_buff, sizeof(send_buff), 0) != sizeof(send_buff)){
            perror("[Error] send() error : ");
            exit(1);
        }

        /* Send Rule Data */
        if(send(sock, rule_data, (int)strlen(rule_data), 0) != (int)strlen(rule_data)){
            perror("[Error] send() error : ");
            exit(1);
        }

        printf("\n");
    }
}

