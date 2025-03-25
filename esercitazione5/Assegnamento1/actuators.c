#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <stdbool.h>
#include "list.h"
#include <time.h>

char *host_name = "127.0.0.1";
int port = 8000;

float randomTempGenerator(float min, float max)
{
    return min + (float)rand() / RAND_MAX * (max - min);
}

int main(int argc, char *argv[])
{
    ItemType msg;
    char type;
    float Tgoal, measure;
    char *listSensors[devices_LENGTH];
    char actuatorName[name_LENGTH];

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <I/D> <Tgoal>\n", argv[0]);
        exit(1);
    }
    else
    {
        type = argv[1];
        Tgoal = atof(argv[2]);
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

    int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        perror("Error creating socket. ");
        exit(1);
    }

    if (connect(sock_fd, (void *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error connecting server.");
        exit(1);
    }

    if (type == "I" || type == "i")
    {
        // procedura di iscrizione
        strncpy(msg.Name, actuatorName, sizeof(actuatorName));
        strncpy(msg.listSensors, listSensors, sizeof(listSensors));
        msg.Tgoal = Tgoal;

        if (send(sock_fd, &msg, sizeof(msg), 0) < 0)
        {
            perror("Error in send.");
            exit(1);
        }

        printf("Waiting for measurement...\n");

        if (recv(sock_fd, &msg, sizeof(msg), 0) < 0)
        {
            perror("Error in recv. ");
            exit(1);
        }

        printf("Measure: %f\n", measure);
    }
    else if (type == "D" || type == "d")
    {
        // procedura di disiscrizione
    }
    else
    {
        printf("Tipo di operazione non riconosciuto.\n");
        exit(1);
    }

    return 0;
}
