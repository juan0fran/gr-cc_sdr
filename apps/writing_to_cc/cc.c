#include "socket_utils.h"

int fd;

void * write_socket(void * args)
{
    unsigned int i = 0;
    char buffer[128];
    printf("Write socket task\n");
    while(1) {
        sprintf(buffer, "Test from USRP %d", i++);
        write(fd, buffer, strlen(buffer));
        usleep((rand()%1500 + 500) * 1000);
    }
}

void * read_socket(void * args)
{
    char c;
    char buffer[128];
    printf("Read socket task\n");
    unsigned int cnt = 0;
    while(1){
        if (read(fd, &c, 1) > 0){
            printf("%02x ", c);
        }
    }
}

int main(void)
{
    pthread_t threads[2];
    int rc;
    fd = socket_init(52001);
    pthread_create(&threads[0], NULL, write_socket, NULL);
    pthread_create(&threads[1], NULL, read_socket, NULL);

    while(1);
}