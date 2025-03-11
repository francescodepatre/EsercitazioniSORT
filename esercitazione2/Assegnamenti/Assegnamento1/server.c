#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#define BUFFER 1000

int port = 8888;

int main(){
    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
    int num, ret;

    int sock_fd = socket(PF_INET, SOCK_STREAM, 0);

    if(sock_fd < 0){
        perror("Errore durante l'apertura socket.");
        exit(501);
    }

    int options = 1;
    if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &options, sizeof(options)) < 0){
        perror("Errore su chiamata setsockopt. ");
        exit(502);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if(bind(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        perror("Errore su chiamata bind. ");
        exit(503);
    }

    if(listen(sock_fd, 10)){
        perror("Errore su chiamata listen. ");
        exit(504);
    }

    socklen_t client_addr_size = sizeof(client_addr);

    int buf;

    while (1)
    {
        printf("Waiting for connection...\n");

        int new_socketfd = accept(sock_fd, (struct sockaddr *)&client_addr, &client_addr_size);
        if(new_socketfd < 0){
            perror("Errore accept. ");
            exit(505);
        }

        if(recv(new_socketfd, &buf, sizeof(buf), 0) < 0){
            perror("Error on recv. ");
            exit(506);
        }

        printf("Received value: %d\n", buf);

        int output_value = buf + 1;
        

        printf("Output value: %d\n", output_value);

        if(send(new_socketfd, &output_value, sizeof(output_value), 0) < 0){
            perror("Error on send. ");
            exit(507);
        }

        close(new_socketfd);
    }
    close(sock_fd);
    return 0;
}