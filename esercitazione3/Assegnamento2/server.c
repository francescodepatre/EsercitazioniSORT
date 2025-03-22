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
	LIST libri;
	LIST lettori; // sockfd

	ItemType msg;
	ItemType *libro;
	ItemType *lettore;

	int answer;

	libri = NewList();
	lettori = NewList();

	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	// int id;
	// LIST bookList;
	// bool availability;

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
		if (msg.tipo == TIPO_LETTORE)
		{
			libro = Find(libri, msg);
			if (libro != NULL)
			{
				// Ho il libro
				printf("Libro trovato\n");
				answer = 1;
				if (send(newsockfd, &answer, sizeof(answer), 0) == -1)
				{
					perror("Error on send");
					exit(1);
				}
				close(newsockfd);
				// aggiorno la disponibilità
				libro->copies -= 1;
				if (libro->copies == 0)
				{
					libri = Dequeue(libri, *libro);
				}
			}
			else
			{
				// Non ho il libro
				msg.sockfd = newsockfd;
				lettori = EnqueueLast(lettori, msg);
			}
		}
		else if (msg.tipo == TIPO_CASAED)
		{
			// gestione casa ed
			answer = 1;
			if (send(newsockfd, &answer, sizeof(answer), 0) == -1)
			{
				perror("Error on send");
				exit(1);
			}

			lettore = Find(lettori, msg);
			printf("Ho trovato un lettore");

			while (msg.copies > 0 && lettore != NULL)
			{
				PrintItem(*lettore);
				if (send(lettore->sockfd, &answer, sizeof(answer), 0) == -1)
				{
					perror("Error on send");
					exit(1);
				}

				lettori = Dequeue(lettori, *lettore);

				msg.copies--;

				if (msg.copies > 0)
				{
					lettore = Find(lettori, msg);
				}
			}
			if (msg.copies > 0)
			{
				libro = Find(libri, msg);
				if (libro == NULL)
				{
					libri = EnqueueLast(libri, msg);
				}
				else
					libro->copies += msg.copies;
			}
			printf("Lista libri\n");
			PrintList(libri);
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
