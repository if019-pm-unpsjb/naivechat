/*
 * Simple chat program using UDP and threads.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_LINE 100

void* send_thread(void* args);
void* recv_thread(void* args);

struct sockaddr_in addr;
socklen_t addr_len = 0;
int sock;

int main(int argc, char *argv[])
{
    pthread_t recvt, sendt;
    char buf[100];

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = (in_port_t) 0;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    bind(sock, (struct sockaddr*) &addr, (socklen_t) sizeof(struct sockaddr_in));

    socklen_t addr_len = (socklen_t) sizeof(addr);
    getsockname(sock, (struct sockaddr*) &addr, &addr_len);
    printf("Listening on: %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

    printf("Send messages to the host: "); fflush(stdout);
    fgets(buf, sizeof(buf), stdin);
    buf[strcspn(buf, "\n")] = '\0'; // remove \n
    if (strlen(buf) > 0) {
        inet_aton(strtok(buf, ":"), &(addr.sin_addr));
        addr.sin_port = htons(atoi(strtok(NULL, ":")));
        printf("The messages will be sended to: %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    }

    printf("^C to exit.\n");

    recvt = pthread_create(&recvt, NULL, recv_thread, NULL);
    sendt = pthread_create(&sendt, NULL, send_thread, NULL);
    pthread_join(recvt, NULL);
    pthread_join(sendt, NULL);
    pthread_exit(NULL);
}

void* send_thread(void* args)
{
    char *buf = malloc(MAX_LINE);
    do {
        fgets(buf, MAX_LINE, stdin);
        sendto(sock, buf, MAX_LINE, 0, (struct sockaddr*) &addr, sizeof(struct sockaddr_in));
    } while (strlen(buf) > 0);

    exit(EXIT_SUCCESS);
}

void* recv_thread(void* args)
{
    char *buf = malloc(MAX_LINE);
    do {
        fflush(stdout);
        recvfrom(sock, buf, MAX_LINE, 0, (struct sockaddr*) &addr, &addr_len);
        printf("[%s:%d]: %s", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), buf);
    } while(strlen(buf) > 0);

    exit(EXIT_SUCCESS);
}
