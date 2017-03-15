//
//  socket_utils.c
//  3CAT-2_COMMS_SOCKET
//
//  Created by Juan Fran Muñoz Martin on 01/10/14.
//  Copyright (c) 2014 Juan Fran Muñoz Martin. All rights reserved.
//

#include "socket_utils.h"

int socket_init(int port){
    int fd;
    struct hostent *he;
    /* estructura que recibirá información sobre el nodo remoto */
    struct sockaddr_in server;
    /* información sobre la dirección del servidor */
    if ((he=gethostbyname("localhost"))==NULL){
        /* llamada a gethostbyname() */
        perror("gethostbyname() error\n");
        exit(1);
    }
    
    if ((fd=socket(AF_INET, SOCK_STREAM, 0))==-1){
        /* llamada a socket() */
        printf("socket() error\n");
        exit(1);
    }
    
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    /* htons() es necesaria nuevamente ;-o */
    server.sin_addr = *((struct in_addr *)he->h_addr);
    /*he->h_addr pasa la información de ``*he'' a "h_addr" */
    bzero(&(server.sin_zero),8);
    
    if(connect(fd, (struct sockaddr *)&server,
               sizeof(struct sockaddr))==-1){
        /* llamada a connect() */
        printf("connect() error\n");
        exit(-1);
    }
    return fd;
}
