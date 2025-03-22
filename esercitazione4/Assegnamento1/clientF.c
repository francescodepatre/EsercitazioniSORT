#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <netdb.h>
#include <stdbool.h>
#include "list.h"

char *host_name = "127.0.0.1"; /* local host */
int port = 8000;

int main(int argc, char *argv[])
{
    char buffer[sizeof(LIST)];
    LIST listaCentri;
    ItemType msg;
    int answer;

    listaCentri = NewList();

    if (argc < 4)
    {
        fprintf("%s <nome> <quantity> <min_requests>\n", argv[0]);
        exit(1);
    }
    else
    {
        strncpy(msg.nome, argv[1], NOME_LENGTH);
        msg.quantity = atoi(argv[2]);
        msg.requests = atoi(argv[3]);
        msg.tipo = TIPO_FORNITORE;
    }

    struct sockaddr_in serv_addr;
    struct hostent *server;

    if ((server = gethostbyname(host_name)) == 0)
    {
        perror("Error resolving local host\n");
        exit(1);
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = ((struct in_addr *)(server->h_addr_list[0]))->s_addr;
    serv_addr.sin_port = htons(port);

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Error opening socket\n");
        exit(1);
    }

    if (connect(sockfd, (void *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("Error connecting to socket\n");
        exit(1);
    }

    printf("Invio messaggio a server\n");
    if (send(sockfd, &msg, sizeof(msg), 0) == -1)
    {
        perror("Error on send\n");
        exit(1);
    }

    printf("Attesa risposta\n");
    if (recv(sockfd, buffer, sizeof(buffer), 0) == -1)
    {
        perror("Error on send\n");
        exit(1);
    }
    memcpy(&listaCentri, buffer, sizeof(LIST));

    PrintList(listaCentri);
    close(sockfd);
    return 0;
}
