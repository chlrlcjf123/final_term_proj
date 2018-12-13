#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket>
#include <arpa/inet.h>

#define PORT 31842
#define SERV_ADDR "127.0.0.1"

struct Rule_Protocol_Header{
    unsigned char Protocol;
    unsigned char Data_len[3];
};

void *firewall_func(void *data){
    printf("Thread 01\n");
}

void *process_func(void *data){
    printf("Thread 02\n");
}

void *sock_recv_func(void *data){
    /* Socket Var */
    int sock=-1;
    struct sockaddr_in serv_addr;

    unsigned char buff_recv[4] = {0};
    int ret_len = 0;
    struct Rule_Protocol_Header *rule_hdr = NULL;

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

    while(1){
        rule_hdr = NULL;

        ret_len = recv(sock, buff_recv, sizeof(buff_recv), 0);
        rule_hdr = (struct Rule_Protocol_Header *)buff_recv;

        if(rule_hdr->Protocol & 1){
            printf("[Info] Firewall Rule");
        } else if(rule_hdr->Protocol & 2){
            printf("[Info] Process Rule");
        } else {
            printf("[Error] The Wrong Approach! ");
        }
    }

    printf("Thread 03\n");
}

int main(void){
    /* Thread Var */
    pthread_t thread_list[3];
    int thread_id = -1;
    int status = -1;

    /* Thread Create */
    thread_id = pthread_create(&thread_list[0], NULL, process_func, NULL);
    if(thread_id < 0){
        perror("[Error] Thread 01 create error : ");
        exit(0);
    }

    thread_id = pthread_create(&thread_list[1], NULL, firewall_func, NULL);
    if(thread_id < 0){
        perror("[Error] Thread 02 create error : ");
        exit(0);
    }

    thread_id = pthread_create(&thread_list[2], NULL, sock_recv_func, NULL);
    if(thread_id < 0){
        perror("[Error] Thread 03 create error : ");
        exit(0);
    }

    pthread_join(thread_list[0], (void**)&status);
    pthread_join(thread_list[1], (void**)&status);
    pthread_join(thread_list[2], (void**)&status);

    return 0;
}

