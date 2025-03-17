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
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	int id;
	LIST bookList;

	int sockfd = socket( PF_INET, SOCK_STREAM, 0 );  
	if ( sockfd == -1 ) 
	{
		perror("Error opening socket");
		exit(1);
	}
	
	int options = 1;
	if(setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, &options, sizeof (options)) < 0) {
		perror("Error on setsockopt");
		exit(1);
	}

	bzero( &serv_addr, sizeof(serv_addr) );
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	if ( bind( sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr) ) == -1 ) 
	{
		perror("Error on binding");
		exit(1);
	}
	
	if ( listen( sockfd, 20 ) == -1 ) 
	{
		perror("Error on listen");
		exit(1);
	}


	socklen_t address_size = sizeof( cli_addr );	
	

	while(1) 
	{
		printf("\nWaiting for a new connection...\n");
				
		int newsockfd = accept( sockfd, (struct sockaddr *)&cli_addr, &address_size );      
		if (newsockfd == -1) 
		{
			perror("Error on accept");
			exit(1);
		}
		
		if ( recv( newsockfd, id, sizeof(id), 0 ) == -1) 
		{
			perror("Error on receive");
			exit(1);
		}

		if(id > 1000){
			char bookTitle[20];
			ItemType* book;
			bool availability;
			if ( recv( newsockfd, bookTitle, strlen(bookTitle), 0 ) == -1) 
				{
					perror("Error on receive");
					exit(1);
				}
			book = Find(bookList, bookTitle);
			if(book != NULL){
				availability = true;
			}
			else{
				availability = false;
			}
			if(send(newsockfd,availability,sizeof(availability)) == -1){
				perror("Error on sending");
				exit(1);
			}
		}
		else{
			char bookTitle[20];
			if ( recv( newsockfd, bookTitle, strlen(bookTitle), 0 ) == -1) 
				{
					perror("Error on receive");
					exit(1);
				}
			book = Find(bookList, bookTitle);
			if(book != NULL){
				availability = true;
			}
			else{
				availability = false;
			}
			if(send(newsockfd,availability,sizeof(availability)) == -1){
				perror("Error on sending");
				exit(1);
			}
		}

		close(newsockfd);
	}

	close(sockfd);
	return 0;
}



