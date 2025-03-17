#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <netdb.h>

char *host_name = "127.0.0.1"; /* local host */
int port = 8000;


int main(int argc, char *argv[]) 
{
	char bookTitle[20];
	int copies;
	int editorID = rand() % 100 + 1;
	
	
	if (argc < 3){ 
		fprintf("Usage: %s <book_title> <copies_num>\nBook title has max length of 20 chars.",argv[0]);
		exit(1);
	}
	else{
		strcpy(bookTitle, argv[1]);
		copies = atoi(argv[2]);
	}
		
	struct sockaddr_in serv_addr;
 	struct hostent* server;	
	
	if ( ( server = gethostbyname(host_name) ) == 0 ) 
	{
		perror("Error resolving local host\n");
		exit(1);
	}

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = ((struct in_addr *)(server->h_addr_list[0]))->s_addr;
	serv_addr.sin_port = htons(port);
	
	int sockfd = socket( PF_INET, SOCK_STREAM, 0 );
	if ( sockfd == -1 ) 
	{
		perror("Error opening socket\n");
		exit(1);
	}    

	if ( connect(sockfd, (void*)&serv_addr, sizeof(serv_addr) ) == -1 ) 
	{
		perror("Error connecting to socket\n");
		exit(1);
	}

	printf("Logging in...\n");
	if ( send(sockfd, editorID, sizeof(editorID), 0) == -1 ) 
	{
		perror("Error on send\n");
		exit(1);
	}


	printf("Sending book title \"%s\" to server...\n", bookTitle);

	if ( send(sockfd, bookTitle, strlen(bookTitle)+1, 0) == -1 ) 
	{
		perror("Error on send\n");
		exit(1);
	}

	printf("Sending book copies to server...\n");

	if ( send(sockfd, copies, sizeof(copies), 0) == -1 ) 
	{
		perror("Error on send\n");
		exit(1);
	}
	
	close(sockfd);

	return 0;
}



