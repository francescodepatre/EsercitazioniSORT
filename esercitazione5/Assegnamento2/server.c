#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <netdb.h>
#include "list.h"
#include <stdbool.h>

int port = 8000;

int main()
{
    LIST guide;
    LIST visitatoriAttesa;

    ItemType msg;
    ItemType response;
    ItemType *guida;

    int found = FALSE;

    guide = NewList();
    visitatoriAttesa = NewList();

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

        msg.sockfd = newsockfd;
        if (msg.type == TYPE_GUIDA)
        {
            if (isEmpty(visitatoriAttesa))
            {
                guide = EnqueueLast(guide, msg);
            }
            else
            {
                if (send(newsockfd, &response, sizeof(response), 0) == -1)
                {
                    perror("Error on send");
                    close(newsockfd);
                    exit(1);
                }
            }
        }
        else if (msg.type == TYPE_VISITATORE)
        {
            if (isEmpty(guide))
            {
                response.avail = RESPONSE_NOT_AVAILABLE;
                if (send(newsockfd, &response, sizeof(response), 0) == -1)
                {
                    perror("Error on send");
                    close(newsockfd);
                    exit(1);
                }
            }
            else
            {
                for (int i = 0; i < getLength(guide); i++)
                {
                    guida = DequeueLast(guide);
                    if ((guida->min_persone <= msg.n_visitors) && (guida->max_persone >= msg.n_visitors))
                    {
                        found = TRUE;
                        response.avail = RESPONSE_OK;
                        if (send(newsockfd, &response, sizeof(response), 0) == -1)
                        {
                            perror("Error on send");
                            close(newsockfd);
                            exit(1);
                        }
                        if (send(guida->sockfd, &response, sizeof(response), 0) == -1)
                        {
                            perror("Error on send");
                            close(newsockfd);
                            exit(1);
                        }
                    }
                    else if (!isEmpty(visitatoriAttesa) && (guida->min_persone >= msg.n_visitors))
                    {
                        ItemType oldVis = getTail(visitatoriAttesa);
                        visitatoriAttesa = DequeueLast(visitatoriAttesa);
                        msg.n_visitors = msg.n_visitors + oldVis.n_visitors;
                    }
                    ItemType pointedGuida;
                    pointedGuida = *guida;
                    guide = EnqueueFirst(guide, pointedGuida);
                }
                if (found == FALSE)
                {
                    visitatoriAttesa = EnqueueLast(visitatoriAttesa, msg);
                }
            }
        }
        else
        {
            printf("Tipo non riconosciuto\n");
            close(newsockfd);
            break;
        }

        close(newsockfd);
    }

    close(sockfd);
    return 0;
}
