//
//  socket_utils.h
//  3CAT-2_COMMS_SOCKET
//
//  Created by Juan Fran Muñoz Martin on 01/10/14.
//  Copyright (c) 2014 Juan Fran Muñoz Martin. All rights reserved.
//

#ifndef __socket_utils__
#define __socket_utils__

#include <stdio.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <arpa/inet.h>
#include <setjmp.h>

#define BACKLOG 5 /* El número de conexiones permitidas */
#define MAXDATASIZE 256

int read_kiss_from_socket(int fd, char * buffer);
int socket_init();

void send_udp(void *p, size_t len);
int recv_udp(void *p, size_t mlen);
#endif 

