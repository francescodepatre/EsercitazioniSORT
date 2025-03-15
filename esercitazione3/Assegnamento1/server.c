/*
    Author: Francesco De Patre
*/

#include "server.h"

int main() {
	srand(time(NULL));
	int sockfd = socket( PF_INET, SOCK_STREAM, 0 ); 

	if(sockfd < 0){
		perror("Error opening socket");
		exit(1);
	}
	
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &options, sizeof (options)) < 0){
		perror("Error on setsockopt");
		exit(1);
	}

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	if (bind( sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		perror("Error on binding");
		exit(1);
	}
	
	if(listen(sockfd, 20) < 0){
		perror("Error on listen");
		exit(1);
	}

	while(1){
		printf("\nWaiting for a new player...\n");
		int newsockfd = accept( sockfd, (struct sockaddr *)&cli_addr, &address_size );

		if (newsockfd < 0){
			perror("Error on accept");
			exit(1);
		}
		n_players++;
		
		if(recv( newsockfd,&player_id, sizeof(player_id), 0) < 0){
			perror("Error on receive");
			exit(1);
		} 

		ItemType* player = Find(historyPlayers, player_id);
        
        if(player != NULL){
            printf("Player %d already registered!\n", player_id);
			connectedPlayers = EnqueueLast(connectedPlayers,*player);
        }
        else{
            printf("Player %d not registered. Creating profile...\n",player_id);
            ItemType* newPlayer = malloc(sizeof(ItemType));
			if(newPlayer == NULL){
				perror("Error memory allocation for player");
				exit(1);
			}
			newPlayer->playerId = player_id;
			newPlayer->playerScore = 0;
			newPlayer->playerMatch = 0;
			connectedPlayers = EnqueueLast(connectedPlayers,*newPlayer);
			historyPlayers = EnqueueLast(historyPlayers, *newPlayer);
        }

		printf("---CONNECTED PLAYERS---\n");
		PrintList(connectedPlayers);
		printf("\n");

        if(n_players == 3){
            printf("3 players connected. Starting game!\n");
            printf("Simulating match");
            for(int i = 0; i < 3; i ++){
                sleep(1);
                printf(".");
            }
            printf("Match finished!");
			shufflePoints(points,3);
			assignPoints(connectedPlayers,points);
        }
		else{
			continue;
		}

		if ( send( newsockfd, &player->playerScore, sizeof(player->playerScore), 0 ) == -1) 
		{
			perror("Error on send");
			exit(1);
		}

		connectedPlayers = DeleteList(connectedPlayers); 

		close(newsockfd);
	}

	close(sockfd);
	return 0;
}


