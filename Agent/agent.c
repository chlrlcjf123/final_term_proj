#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define BUFF_SIZE 4096

typedef struct {
    long data_type;
    int sub_type;
    char data_buff[BUFF_SIZE];
} t_data;

int msqid;

/* Firewall Thread Func */
void *firewall_func(void *data){
    t_data recv_data;
    char *ip_addr=NULL;
    char *port=NULL;
    char run_cmd[500]={0};

    printf("[Info] Firewall Thread Run\n");
    while(1){
        memset(run_cmd, 0, sizeof(run_cmd));
        memset(&recv_data, 0, sizeof(recv_data));

        /* Get Firewall Rule from Message Queue */
        if( -1 == msgrcv(msqid, &recv_data, sizeof(t_data) - sizeof(long), 1, 0)){
            perror( "msgrcv() 실패");
            exit(1);
        }

        if(recv_data.sub_type == 1){
            char *ptr = strtok(recv_data.data_buff, ";");
            ip_addr = ptr;

            while(ptr = strtok(NULL, ";")){
                port=ptr;
            }

            printf("[Info] Firewall Rule Add (IP:%s, Port:%s)\n",ip_addr, port);
            sprintf(run_cmd, "iptables -I INPUT 1 -p tcp -s %s --dport %s -j ACCEPT", ip_addr, port);

        } else {
            printf("[Info] Firewall Rule Delete (Rule Num:%s)\n",recv_data.data_buff);
            sprintf(run_cmd, "iptables -D INPUT %s", recv_data.data_buff);
        }
        system(run_cmd);
    }
}

/* Process Thread Func*/
void *process_func(void *data){
    t_data recv_data;
    char run_cmd[500] = {0};

    printf("[Info] Process Thread Run\n");

    while(1){
        /* Get Process Rule from Message Queue */
        if( -1 == msgrcv(msqid, &recv_data, sizeof(t_data) - sizeof(long), 2, 0)){
            perror( "msgrcv() 실패");
            exit(1);
        }

        if(recv_data.sub_type == 1){
            printf("[Info] Process Rule Add\n");
            //Process Block code here
        } else {
            printf("[Info] Process Rule Delete\n");
            //Process Block Delete code here
        }
    }

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

