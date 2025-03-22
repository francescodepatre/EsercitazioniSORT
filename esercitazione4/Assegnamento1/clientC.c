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
    ItemType fornitore;
    ItemType msg;
    int answer;

    if (argc < 3)
    {
        fprintf("%s <nome> <patients>\n", argv[0]);
        exit(1);
    }
    else
    {
        strncpy(msg.nome, argv[1], NOME_LENGTH);
        msg.pazienti = atoi(argv[2]);
        msg.tipo = TIPO_CENTRO;
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
    if (recv(sockfd, &fornitore, sizeof(fornitore), 0) == -1)
    {
        perror("Error on send\n");
        exit(1);
    }

    PrintItem(fornitore);
    close(sockfd);
    return 0;
}
