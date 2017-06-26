#include "socket_utils.h"
#include "link_layer.h"
#include "of_reed-solomon_gf_2_m.h"

uint8_t cmp_buffer[1500];

static int fd;

static int cnt_rx = 0;
static int phy_rx_count = 0;

chunk_handler_t send_packet_hchunk;

void send_packet(void)
{
    link_layer_packet_t ll_packet;
	radio_packet_t radio_packet;
    ll_packet.fields.attribs = 9600;
    ll_packet.fields.len = 1500;
    ll_packet.fields.crc = 0;
    memset(ll_packet.fields.payload, 0xAA, sizeof(ll_packet.fields.payload));
	/* Init done */
	while (get_new_packet_from_chunk(&send_packet_hchunk, ll_packet.raw, (ll_packet.fields.len + LINK_LAYER_HEADER_SIZE),
										4, &radio_packet) > 0)
	{
		write(fd, &radio_packet, MAC_UNCODED_PACKET_SIZE);
	}
	write(fd, &radio_packet, MAC_UNCODED_PACKET_SIZE);
}

void receive_packet(void)
{
    link_layer_packet_t ll_packet;
    chunk_handler_t hchunk;
    radio_packet_t radio_packet;
    int ret;
    init_chunk_handler(&hchunk);
    while(1) {
        if ( (ret = read(fd, &radio_packet, MAC_UNCODED_PACKET_SIZE) ) > 0) {
            phy_rx_count++;
            printf("Phy layer count: %d\n", phy_rx_count);
            if(set_new_packet_to_chunk(&hchunk, &radio_packet, ll_packet.raw) > 0) {
                if (ll_packet.fields.len == 1500) {
                    cnt_rx++;
                    printf("Received a link layer packet. Count: %d\n", cnt_rx);
                    return;
                }
            }
        }
    }
}

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

void transmitter_work(void)
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
		}
		write(fd, &radio_packet, MAC_UNCODED_PACKET_SIZE);
        cnt++;
        printf("LL Packet Sent: %d total\n", cnt);
        sleep(6);
	}
}

int main(int argc, char *argv[])
{
    pthread_t thread;
    char opt;
    int cnt = 0;
    if (argc != 2) {
        printf("BAD SINTAX\n");
        return -1;
    }
    opt = argv[1][0];
    fd = socket_init(52001);
    if (opt == 't') {           /* packet sending */
        transmitter_work();
    }else if (opt == 'r') {     /* packet receiving */
        receiver_work(NULL);
    }else if (opt == 'x') {     /* packet retransmit */
        init_chunk_handler(&send_packet_hchunk);
        while(1) {
            receive_packet();
            /* wait 1 second and retransmit */
            sleep(2);
            send_packet();
            cnt++;
            printf("Sending packet: %d\r\n", cnt);
        }
    }else if (opt == 'w') {     /* packet send and wait */
        init_chunk_handler(&send_packet_hchunk);
        pthread_create(&thread, NULL, receiver_work, NULL);
        while(cnt++ < 100) {
            printf("Sending packet: %d\r\n", cnt);
            send_packet();
            sleep(10);
        }
    }
    close(fd);
    return 0;
}
