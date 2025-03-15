#ifndef _CLIENT_H
#define _CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

char *host_name = "127.0.0.1";
int port = 8000;
int id;
int matchScore;
struct sockaddr_in serv_addr;
struct hostent* server;	

#endif
