#include "socket_utils.h"
#include "link_layer.h"
#include "of_reed-solomon_gf_2_m.h"

#define __OF_TEST__

int fd;

#ifndef __OF_TEST__

void * write_socket(void * args)
{
    unsigned int i = 0;
    char buffer[128];
    printf("Write socket task\n");
    while(1) {
        sprintf(buffer, "Test from USRP %d", i++);
        write(fd, buffer, strlen(buffer));
        usleep((rand()%500 + 500) * 1000);
    }
}


void * read_socket(void * args)
{
    char c;
    char buffer[128];
    printf("Read socket task\n");
    unsigned int cnt = 0;
    while(1){
        if (read(fd, &c, 1) ){
            if (c == '\n'){
                printf(" Cnt: %d\n", cnt++);
            }else{
                printf("%c", c);
            }
        }
    }
}

#else

uint8_t cmp_buffer[1500];

void * receiver_work(void *args)
{
	link_layer_packet_t ll_packet;
	radio_packet_t radio_packet;
	chunk_handler_t hchunk;
	int ret, i;
    int phy_rx_count = 0;
	init_chunk_handler(&hchunk);
    int cnt = 0;
    for (i = 0; i < 1500; i++) {
        cmp_buffer[i] = i % 256;
    }
	/* Init done */
	while(1) {
		/* Wait for notification here */
		if ( (ret = read(fd, &radio_packet, MAC_UNCODED_PACKET_SIZE) ) > 0) {
            phy_rx_count++;
            printf("Phy layer count: %d\n", phy_rx_count);
			if(set_new_packet_to_chunk(&hchunk, &radio_packet, ll_packet.raw) > 0) {
                if (ll_packet.fields.len == 1500) {
                    cnt++;
                    printf("Received a link layer packet. Count: %d\n", cnt);
                }
			}
		}
	}
}

void * transmitter_work(void *args)
{
	link_layer_packet_t ll_packet;
	radio_packet_t radio_packet;
	chunk_handler_t hchunk;
	init_chunk_handler(&hchunk);
    int cnt;
    ll_packet.fields.attribs = 9600;
    ll_packet.fields.len = 1500;
    ll_packet.fields.crc = 0;
    memset(ll_packet.fields.payload, 0xAA, sizeof(ll_packet.fields.payload));
	/* Init done */
    cnt = 0;
	while(1) {
		while (get_new_packet_from_chunk(&hchunk, ll_packet.raw, (ll_packet.fields.len + LINK_LAYER_HEADER_SIZE),
											4, &radio_packet) > 0)
		{
			write(fd, &radio_packet, MAC_UNCODED_PACKET_SIZE);
            //usleep(200 * 1000);
		}
		write(fd, &radio_packet, MAC_UNCODED_PACKET_SIZE);
        //usleep(200 * 1000);
        cnt++;
        printf("LL Packet Sent: %d total\n", cnt);
        sleep(6);
	}
}

#if 0
void * write_socket(void * args)
{
    uint8_t symb_reserved_space_tx[OF_MAX_ENCODING_SYMBOLS * OF_MAX_SYMBOL_SIZE];
    uint8_t symb_reserved_space_rx[OF_MAX_ENCODING_SYMBOLS * OF_MAX_SYMBOL_SIZE];
    chunk_handler_t chunk_tx;
    radio_packet_t packet;
    uint8_t buffer[256];
    int ret, i;
    init_chunk_handler(&chunk_tx);
    printf("Write socket task\n");

    /* Sending! */
    for (i = 0; i < sizeof(symb_reserved_space_tx); i++) {
        symb_reserved_space_tx[i] = i%256;
    }
    while (1) {
        while ( (ret = get_new_packet_from_chunk(&chunk_tx, symb_reserved_space_tx, 1500, 2, &packet) ) >= 0){
            memcpy(buffer, packet.raw, sizeof(packet.raw));
            write(fd, buffer, MAC_UNCODED_PACKET_SIZE);
            if (ret == 0)
                break;
        }
        sleep(10);
    }
}

void * read_socket(void * args)
{
    uint8_t symb_reserved_space_tx[OF_MAX_ENCODING_SYMBOLS * OF_MAX_SYMBOL_SIZE];
    uint8_t symb_reserved_space_rx[OF_MAX_ENCODING_SYMBOLS * OF_MAX_SYMBOL_SIZE];
    radio_packet_t packet;
    link_layer_packet_t ll_packet;
    uint8_t buffer[256];
    int ret;
    printf("Read socket task\n");

    uint8_t current_packet = 0;
    bool decoded_done = false;
    uint32_t cnt = 0;

    int i, j;

    chunk_handler_t chunk_rx;
    init_chunk_handler(&chunk_rx);
    memset(symb_reserved_space_tx, 0xAA, sizeof(symb_reserved_space_tx));

    while(1){
        if ( (ret = read(fd, buffer, MAC_UNCODED_PACKET_SIZE) ) > 0){
            memcpy(packet.raw, buffer, ret);
            ret = set_new_packet_to_chunk(&chunk_rx, &packet, symb_reserved_space_rx);
            if (ret > 0) {
                cnt++;
                ll_packet = (link_layer_packet_t *) symb_reserved_space_rx;
                printf("Link layer packet received: %d\n", ll_packet->fields.len);
                printf("Received a whole chunk: %d received until now\n", cnt);
                for (i = 0, j = 0; i < ret; i++){
                    if (symb_reserved_space_rx[i] == i%256){
                        j++;
                    }
                }
                printf("Size of chunk is: %d and contains %d corrects\n", ret, j);
            }
        }else{
            return NULL;
        }
    }
}
#endif
#endif

int main(int argc, char *argv[])
{
    char opt;
    if (argc != 2) {
        printf("BAD SINTAX\n");
        return -1;
    }
    opt = argv[1][0];
    if (opt == 't') {
        fd = socket_init(52001);
        transmitter_work(NULL);
        close(fd);
    }else if (opt == 'r') {
        fd = socket_init(52001);
        receiver_work(NULL);
        close(fd);
    }else {
        printf("Bad option\n");
    }
    /**
    pthread_t threads[2];
    int rc;

    transmitter_work(NULL);
    close(fd);

    pthread_create(&threads[0], NULL, transmitter_work, NULL);
    //pthread_create(&threads[1], NULL, receiver_work, NULL);
    //while(1);
    */
    return 0;
}
