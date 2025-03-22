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
    LIST centri;
    LIST fornitori;
    LIST centriOutput;

    ItemType msg;
    ItemType *centro;
    ItemType *fornitore;

    int answer;

    centri = NewList();
    fornitori = NewList();
    centriOutput = NewList();

    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    int id;
    LIST bookList;
    bool availability;

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

        // QUI ricevo il messaggio
        if (msg.tipo == TIPO_CENTRO)
        {
            msg.sockfd = newsockfd;
            if (getLength(fornitori) != 0)
            {
                LIST tmp = fornitori;
                while (!isEmpty(tmp))
                {
                    // controllo se il fornitore può soddisfare la richiesta - DA MIGLIORARE
                    if (msg.requests < tmp->item.quantity)
                    {
                        if (send(newsockfd, &tmp, sizeof(tmp), 0) == -1)
                        {
                            perror("Error on send");
                            exit(1);
                        }
                        break;
                    }
                    tmp = tmp->next;
                }
            }
            else
            {
                centri = EnqueueLast(centri, msg);
            }
        }
        else if (msg.tipo == TIPO_FORNITORE)
        {
            msg.sockfd = newsockfd;
            if (getLength(centri) != 0)
            {
                LIST tmp = centri;
                while (!isEmpty(tmp))
                {
                    // controllo se il fornitore può soddisfare la richiesta - DA MIGLIORARE
                    if (msg.quantity > tmp->item.requests)
                    {
                        centriOutput = EnqueueFirst(centriOutput, tmp->item);
                        if (send(newsockfd, &centriOutput, sizeof(centriOutput), 0) == -1)
                        {
                            perror("Error on send");
                            exit(1);
                        }
                        break;
                    }
                    tmp = tmp->next;
                }
            }
            else
            {
                fornitori = EnqueueLast(fornitori, msg);
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
