#define PORT 31842
#define SERV_ADDR "127.0.0.1"

struct Rule_Protocol_Header{
    unsigned char Protocol;
    unsigned char Data_len[3];
};