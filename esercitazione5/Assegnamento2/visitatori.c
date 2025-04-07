#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <stdbool.h>
#include "list.h"

char *host_name = "127.0.0.1";
int port = 8000;

int main(int argc, char *argv[])
{
    ItemType msg;
    ItemType reply;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <n_pers>\n", argv[0]);
        exit(1);
    }
    else
    {
        msg.n_visitors = atoi(argv[1]);
        msg.type = TYPE_VISITATORE;
    }

    struct sockaddr_in serv_addr;
    struct hostent *server;

    if ((server = gethostbyname(host_name)) == 0)
    {
        perror("Error resolving local host");
        exit(1);
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = ((struct in_addr *)(server->h_addr_list[0]))->s_addr;
    serv_addr.sin_port = htons(port);

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Error opening socket");
        close(sockfd);
        exit(1);
    }

    if (connect(sockfd, (void *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("Error connecting to socket");
        close(sockfd);
        exit(1);
    }

    if (send(sockfd, &msg, sizeof(msg), 0) == -1)
    {
        perror("Error on send");
        close(sockfd);
        exit(1);
    }

    printf("Waiting...\n");
    if (recv(sockfd, &reply, sizeof(reply), 0) == -1)
    {
        perror("Error on recv");
        close(sockfd);
        exit(1);
    }

    if (reply.avail == RESPONSE_OK)
    {
        printf("Nome guida: %s\n", reply.nome);
    }
    else if (reply.avail == RESPONSE_NOT_AVAILABLE)
    {
        printf("Nessuna guida disponibile...\n");
    }

    close(sockfd);
    return 0;
}
