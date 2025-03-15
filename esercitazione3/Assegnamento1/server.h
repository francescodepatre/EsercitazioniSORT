#ifndef _SERVER_H
#define _SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>

#include "list.h"

int port = 8000;
struct sockaddr_in serv_addr;
struct sockaddr_in cli_addr;
int n_players = 0;
int player_id;
int matchResult; 
int options = 1;
socklen_t address_size = sizeof( cli_addr );	
LIST historyPlayers;
LIST connectedPlayers;
int points[] = {1, 2, 3};
#endif