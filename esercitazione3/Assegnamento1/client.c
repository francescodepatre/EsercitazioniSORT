/*
    Author: Francesco De Patre
*/

#include "client.h"

int main(int argc, char *argv[]) 
{
	
	
	if(argc < 2){
		printf("Usage: %s <id_player>\n",argv[0]);
    }
	else{
		id = atoi(argv[1]);
    }
	
    if((server = gethostbyname(host_name))== 0){
		perror("Error resolving local host\n");
		exit(1);
	}

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = ((struct in_addr *)(server->h_addr_list[0]))->s_addr;
	serv_addr.sin_port = htons(port);

	int sockfd = socket( PF_INET, SOCK_STREAM, 0 );
	
	if (sockfd < 0){
		perror("Error opening socket\n");
		exit(1);
	}    

	if(connect(sockfd, (void*)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("Error connecting to socket\n");
		exit(1);
	}

	printf("Connected to server!\n");
    printf("Sending player id...\n");

	if(send(sockfd, &id, sizeof(id), 0) < 0){
		perror("Error on send\n");
		exit(1);
	}

	printf("Player id sent. Waiting for match...\n");
	
	if(recv(sockfd, &matchScore, sizeof(matchScore), 0) < 0) {
		perror("Error in receiving response from server\n");
		exit(1);
	}

	printf("Player match score: \"%d\n", matchScore);

	close(sockfd);

	return 0;
}


