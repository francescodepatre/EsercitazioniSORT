/*
Implement a Client that perform multiple concurrently requests

Each Client must create multiple concurrent instances.
Each instance generates a unique number and sends it to the server, receives a response and prints the sent and the received number.

Server simply take the number and add 1.

unique seed: srand(time(NULL) + ??? )
use sleep() to see client child processes prints (or loop wait())
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1000

char *hostname = "127.0.0.1";
int port = 8888;

int main(int argc, char *argv[]){
    struct sockaddr_in server_address;
    struct hostent* server;
    int answer, child_n = atoi(argv[1]);
    pid_t pid;

    if(argc < 2){
        perror("Usage: ./client <child_process_number> ");
        exit(400);
    }

    if(!(server = gethostbyname(hostname))){
        perror("Error resolving localhost. ");
        exit(401);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = ((struct in_addr *)(server->h_addr_list[0]))->s_addr;
    server_address.sin_port = htons(port);

    //qui la generazione dei figli
    for(int i = 0; i < child_n; i++){
        pid = fork();

        if(pid < 0){
            perror("Error on calling fork. ");
            exit(404);
        }
        else if(pid == 0){
            srand(time(NULL) + getpid());
            int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
            if(sock_fd < 0){
                perror("Error creating socket. ");
                exit(402);
            }

            if(connect(sock_fd, (void *)&server_address, sizeof(server_address)) < 0){
                perror("Error connecting server.");
                exit(403);
            }
            
            printf("Process child %d with pid %d created! \n", i, getpid());
            int number = rand() % 100 + 1;
            printf("%d:Sending %d to server.\n",getpid(),number);

            if(send(sock_fd, &number, sizeof(number), 0) < 0){
                perror("Failed to send number. ");
                exit(404);
            }

            printf("Waiting for response...\n");

            if(recv(sock_fd, &answer, sizeof(answer), 0) < 0){
                perror("Failed to receive number. ");
                exit(405);
            }
            close(sock_fd);
            printf("Response from server: %d\n", answer);
            exit(0);
        }
    }

    for(int i = 0; i < child_n; i++){
        wait(NULL);
    }
    
    return 0;
    
}