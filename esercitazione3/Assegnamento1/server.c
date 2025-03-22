#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <netdb.h>

#include "list.h"

#define NUM_PLAYERS 3

int port = 8000;

int main()
{
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

	// Maximum number of connection kept in the socket queue
	if (listen(sockfd, 20) == -1)
	{
		perror("Error on listen");
		exit(1);
	}

	socklen_t address_size = sizeof(cli_addr);

	// QUI GETISCO LE CONNESSIONI IN INGRESSO

	itemType msg;

	srand(time(NULL)); // per estrazione classifica casuale

	LIST ranking = NewList();		 // tiene lo storico
	LIST active_players = NewList(); // registra i giocatori attuali

	while (1)
	{
		printf("\nWaiting for a new player...\n");

		// New connection acceptance
		int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &address_size);
		if (newsockfd == -1)
		{
			perror("Error on accept");
			exit(1);
		}

		// Message reception
		if (recv(newsockfd, &msg, sizeof(itemType), 0) == -1)
		{
			perror("Error on receive");
			exit(1);
		}

		printf("\nReceived msg from player %s: joined the match\n", msg.nome);

		// CONTROLLARE SE IL PLAYER È NUOVO O MENO
		itemType *player = Find(ranking, msg);

		if (player == NULL)
		{
			printf("Player '%s' connected for the first time\n", msg.nome);
			// Ribadisco tutto a 0, per precauzione
			msg.num_partite = 0;
			msg.punteggio = 0;
			msg.sockfd = 0;
			ranking = EnqueueLast(ranking, msg);
		}
		else
		{
			printf("Player '%s' has already played %d matches\n", player->nome, player->num_partite); // cercato nella lista ranking
		}

		msg.sockfd = newsockfd;							   // socket del player attuale
		active_players = EnqueueLast(active_players, msg); // inserisco nella lista del match

		printf("Players waiting for the game:\n");

		// Devo stampare le info dei nodi di "ranking" che ottengo dalla lista "active_players"
		LIST it; // puntatore temporaneo
		for (it = active_players; it != NULL; it = it->next)
		{
			player = Find(ranking, it->item);
			PrintItem(*player);
		}

		if (getLength(active_players) >= NUM_PLAYERS)
		{
			printf("\nPlaying the game...\n");

			// Increasing number of matches
			for (it = active_players; it != NULL; it = it->next)
			{
				player = Find(ranking, it->item);
				player->num_partite++;
			}

			// Classifica causale
			int points;
			for (points = 3; points > 0; --points)
			{

				/* Deciding the winner */
				int remainingPlayers = getLength(active_players);
				int winningIndex = rand() % remainingPlayers;
				itemType winningPlayer = getItemAtPos(active_players, winningIndex);

				/* Attributing points */
				player = Find(ranking, winningPlayer);
				player->punteggio += points;

				printf("Player %s wins %d points. Total score: %d\n", player->nome, points, player->punteggio);

				/* Sending the result of the match */
				if (send(winningPlayer.sockfd, &points, sizeof(int), 0) == -1)
				{
					perror("Error sending the result to a winner");
					exit(1);
				}
				// IMPORTANTE: chiusura socket e rimozione dalla lista
				close(winningPlayer.sockfd);
				active_players = Dequeue(active_players, *player);
			}

			/* Remaining players (losers) */
			while (!isEmpty(active_players))
			{
				itemType loser = getHead(active_players);
				printf("Player %s lose\n", loser.nome);

				int points = 0;
				if (send(loser.sockfd, &points, sizeof(int), 0) == -1)
				{
					perror("Error sending the result to a loser");
					exit(1);
				}
				close(loser.sockfd);
				active_players = DequeueFirst(active_players);
			}
			printf("\nRanking:\n");
			PrintList(ranking);
		}
	}

	close(sockfd);
	return 0;
}
