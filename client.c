#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

#define BUF_SIZE 1024
void error_handling(char *message);
void * recv_msg(void * arg);
void * send_msg(void * arg);

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;
    void * thread_return;

    if(argc!=3)
    {
        printf("Usage : %s <IP> <port> \n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock ==-1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[2]));

    if(connect(sock, (struct  sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("bind() error");
    else
        puts("connected..............");

    pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);
    close(sock);
    return 0;
}

void * send_msg(void * arg)
{
    int sock=*((int*)arg);
    char msg[BUF_SIZE];
    while(1)
    {
        fputs("Input message(Q to quit): ", stdout);
        fgets(msg, BUF_SIZE, stdin);

        if(!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
        {
            close(sock);
            exit(0);
        }

        write(sock, msg, strlen(msg));
    }
    return NULL;
}

void * recv_msg(void * arg)
{
    int sock = *((int*)arg);
    char msg[BUF_SIZE];
    int str_len;
    while(1)
    {
        str_len=read(sock, msg, BUF_SIZE-1);
        if(str_len==-1)
            return (void*) -1;
        msg[str_len]=0;
        printf("Message from server: %s\n", msg);
    }
    return NULL;
}
void error_handling(char *message)
{
    perror(message);
    exit(1);
}