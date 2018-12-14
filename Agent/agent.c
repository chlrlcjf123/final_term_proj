#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define  BUFF_SIZE   1024

typedef struct {
    long data_type;
    int sub_type;
    char data_buff[BUFF_SIZE];
} t_data;

int msqid;

void *firewall_func(void *data){
    t_data recv_data;

    while(1){
        if( -1 == msgrcv(msqid, &recv_data, sizeof(t_data) - sizeof(long), 1, 0)){
            perror( "msgrcv() 실패");
            exit(1);
        }
        printf( "%ld - %d - %s\n", recv_data.data_type, recv_data.sub_type, recv_data.data_buff);
    }
}

void *process_func(void *data){
    printf("Thread 02\n");
}

int main(void){
    /* Thread Var */
    pthread_t thread_list[3];
    int thread_id = -1;
    int status = -1;

    if( -1 == (msqid = msgget( (key_t)3856, IPC_CREAT | 0666))){
        perror("msgget() 실패");
        exit(1);
    }

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

    pthread_join(thread_list[0], (void**)&status);
    pthread_join(thread_list[1], (void**)&status);

    return 0;
}

