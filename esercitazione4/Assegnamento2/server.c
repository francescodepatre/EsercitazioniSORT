#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <netdb.h>
#include "list.h"

int port = 8000;

void updatePrice(LIST l, ItemType item)
{
    LIST tmp = l;
    while (tmp != NULL)
    {
        if (itemCompare(tmp->item, item) == 0)
        {
            tmp->item.price += 1;
        }
        else
        {
            tmp->item.price -= 1;
        }
        tmp = tmp->next;
    }
}

void sendList(LIST l, int sock)
{
    int length = getLength(l);
    LIST tmp = l;
    if (send(sock, &length, sizeof(length), 0) == -1)
    {
        perror("Error on send");
        exit(1);
    }

    for (int i = 0; i < length; i++)
    {
        if (send(sock, &tmp->item, sizeof(tmp->item), 0) == -1)
        {
            perror("Error on send");
            exit(1);
        }
        tmp = tmp->next;
    }
}

int main()
{
    LIST stocks;

    ItemType *stock;
    ItemType msg;
    ItemType choice;

    stocks = NewList();

    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Error opening socket");
        exit(1);
    }

    int options = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &options, sizeof(options)) < 0)
    {
        perror("Error on setsockopt");
        exit(1);
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("Error on binding");
        exit(1);
    }

    if (listen(sockfd, 20) == -1)
    {
        perror("Error on listen");
        exit(1);
    }

    socklen_t address_size = sizeof(cli_addr);

    while (1)
    {
        printf("\nWaiting for a new connection...\n");
        if (stocks != NULL)
        {
            PrintList(stocks);
        }
        else
        {
            printf("List is null!");
        }

        int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &address_size);
        if (newsockfd == -1)
        {
            perror("Error on accept");
            exit(1);
        }

        if (recv(newsockfd, &msg, sizeof(msg), 0) == -1)
        {
            perror("Error on receive");
            exit(1);
        }

        if (msg.type == TYPE_AGENTE)
        {
            printf("Agent connected!\n");
            msg.sockfd = newsockfd;
            stocks = EnqueueLast(stocks, msg);
            printf("Agent added!\n");
        }
        else if (msg.type == TYPE_INVESTOR)
        {
            printf("Investor connected!\n");
            msg.sockfd = newsockfd;
            sendList(stocks, newsockfd);

            if (recv(newsockfd, &choice, sizeof(choice), 0) == -1)
            {
                perror("Error on receive");
                exit(1);
            }
            close(newsockfd);

            stock = Find(stocks, choice);
            updatePrice(stocks, choice);
            if ((stock->quantity == 0) || (stock->price < stock->minPrice))
            {
                Dequeue(stocks, *stock);
                stock->ricavi += stock->price;
                stock->quantity--;
                if (send(stock->sockfd, stock, sizeof(stock), 0) == -1)
                {
                    perror("Error on send");
                    exit(1);
                }
                close(stock->sockfd);
            }
            else if (stock == NULL)
            {
                printf("NOT FOUND");
                close(newsockfd);
            }
            else
            {
                stock->ricavi += stock->price;
                stock->quantity--;
            }
        }
        else
        {
            printf("Tipo non riconosciuto\n");
            close(newsockfd);
            break;
        }

        // close(newsockfd);
    }

    close(sockfd);
    return 0;
}
