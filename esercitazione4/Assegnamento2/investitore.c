#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <stdbool.h>
#include "list.h"

char *host_name = "127.0.0.1";
int port = 8000;

LIST receiveList(int sock)
{
    int length = 0;
    ItemType item;
    LIST tmp = NewList();
    if (recv(sock, &length, sizeof(length), 0) == -1)
    {
        perror("Error on send");
        exit(1);
    }

    for (int i = 0; i < length; i++)
    {
        if ((recv(sock, &item, sizeof(item), 0)) == -1)
        {
            perror("Error on recv");
            exit(1);
        }
        tmp = EnqueueLast(tmp, item);
    }
    return tmp;
}

int main(int argc, char *argv[])
{
    LIST stocks;
    ItemType msg;
    ItemType stockChoice;
    char stockName[AGENTE_LENGTH];

    stocks = NewList();
    struct sockaddr_in serv_addr;
    struct hostent *server;

    msg.type = TYPE_INVESTOR;

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

    printf("Waiting stocks list...\n");

    stocks = receiveList(sockfd);

    printf("Stocks available:\n");
    PrintList(stocks);

    printf("Enter stock name:\n");
    scanf("%s", stockName);

    strncpy(stockChoice.agenteName, stockName, sizeof(stockName));

    if (send(sockfd, &stockChoice, sizeof(stockChoice), 0) == -1)
    {
        perror("Error on send");
        close(sockfd);
        exit(1);
    }

    printf("Stock purchased!\nClosing...");
    close(sockfd);
    return 0;
}
