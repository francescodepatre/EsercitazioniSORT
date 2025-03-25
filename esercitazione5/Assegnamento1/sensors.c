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
    ItemType measure;
    int Nt = 0;
    int temp = 0;
    float minTemp = -5.0, maxTemp = 36.0;
    pid_t pid = 0;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <Nt>\n", argv[0]);
        exit(1);
    }
    else
    {
        Nt = atoi(argv[1]);
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

    for (int i = 0; i < Nt; i++)
    {
        pid = fork();

        if (pid < 0)
        {
            perror("Error on calling fork. ");
            exit(1);
        }
        else if (pid == 0)
        {
            srand(time(NULL) + getpid());
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

            printf("Process child %d with pid %d created! \n", i, getpid());
            int waitingTime = (rand() % 6) * 1000;
            printf("%d:Sending waiting %d seconds.\n", getpid(), waitingTime);

            sleep(waitingTime);

            float temp = randomTempGenerator(minTemp, maxTemp);

            printf("%d: Temperature measured -> %d °C\n", getpid(), temp);

            measure.temp = temp;

            if (send(sock_fd, &measure, sizeof(measure), 0) < 0)
            {
                perror("Error in send.");
                exit(1);
            }

            printf("%d: Waiting for response...\n", getpid());

            if (recv(sock_fd, &measure, sizeof(measure), 0) < 0)
            {
                perror("Error in recv. ");
                exit(1);
            }
            close(sock_fd);
            printf("%d: Number of actuators -> %d\n", getpid(), measure.nRecived);
            exit(1);
        }
        else
        {
            wait(NULL);
        }
    }
    return 0;
}
