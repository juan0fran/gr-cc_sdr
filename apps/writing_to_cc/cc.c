#include "socket_utils.h"
#include "link_layer.h"
#include "of_reed-solomon_gf_2_m.h"

/* COMPILE WITH:

gcc cc.c link_layer.c of_reed-solomon_gf_2_m_api.c socket_utils.c galois_field_codes_utils/*.
        -I./galois_field_codes_utils/ -I. -lpthread

*/

typedef struct metadata_u{
    size_t channel;
    bool has_time_spec;
    uint8_t time_spec_array[23];
    uint32_t event_code;
    uint32_t user_payload[4];
}metadata_u;

#define BLIND_TEST_DURATION         1000
#define RETRANSMIT_TEST_DURATION    500

uint8_t cmp_buffer[1740];

static int fd;
static int feedback_fd;

static int cnt_rx = 0;
static int phy_rx_count = 0;
static int phy_tx_count = 0;

chunk_handler_t send_packet_hchunk;

static uint8_t ctrl_val = 0xAA;

void send_packet(void)
{
    metadata_u metadata;
    int i;
    link_layer_packet_t ll_packet;
    radio_packet_t radio_packet;
    ll_packet.fields.attribs = 9600;
    ll_packet.fields.len = 1740;
    ll_packet.fields.crc = 0;
    for (i = 0; i < 1740; i++) {
        ll_packet.fields.payload[i] = i % 256;
    }
    /* Init done */
    send_udp(&ctrl_val, 1);
    while (get_new_packet_from_chunk(&send_packet_hchunk, ll_packet.raw,
            (ll_packet.fields.len + LINK_LAYER_HEADER_SIZE), 4, &radio_packet) > 0) {
        send_udp(&radio_packet, MAC_UNCODED_PACKET_SIZE);
    }
    send_udp(&radio_packet, MAC_UNCODED_PACKET_SIZE);
    read(feedback_fd, &metadata, sizeof(metadata));
    printf("Received event code: %d\n", metadata.event_code);
    while (metadata.event_code != 2) {
        read(feedback_fd, &metadata, sizeof(metadata));
        printf("Received event code: %d\n", metadata.event_code);
    }
    phy_tx_count += 12;
    printf("Total physical layer transmitter packets: %d\n", phy_tx_count);
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
            if(set_new_packet_to_chunk(&hchunk, &radio_packet, ll_packet.raw) > 0) {
                if (ll_packet.fields.len == 1740) {
                    cnt_rx++;
                    printf("Total physical layer received count: %d\n", phy_rx_count);
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
    for (i = 0; i < 1740; i++) {
        cmp_buffer[i] = i % 256;
    }
    /* Init done */
    while(cnt < BLIND_TEST_DURATION) {
    /* Wait for notification here */
    if ( (ret = read(fd, &radio_packet, MAC_UNCODED_PACKET_SIZE) ) > 0) {
            phy_rx_count++;
            if(set_new_packet_to_chunk(&hchunk, &radio_packet, ll_packet.raw) > 0) {
                if (ll_packet.fields.len == 1740) {
                    cnt++;
                    printf("Total physical layer received count: %d\n", phy_rx_count);
                    printf("Received a link layer packet. Count: %d\n", cnt);
                }
            }
        }
    }
}

void transmitter_work(void)
{
    metadata_u metadata;
    link_layer_packet_t ll_packet;
    radio_packet_t radio_packet;
    chunk_handler_t hchunk;
    init_chunk_handler(&hchunk);
    int cnt;
    int i;
    ll_packet.fields.attribs = 9600;
    ll_packet.fields.len = 1740;
    ll_packet.fields.crc = 0;
    for (i = 0; i < 1740; i++) {
        ll_packet.fields.payload[i] = i % 256;
    }
    /* Init done */
    cnt = 0;
    while(cnt < BLIND_TEST_DURATION) {
        send_udp(&ctrl_val, 1);
        while (get_new_packet_from_chunk(&hchunk, ll_packet.raw,
                                            (ll_packet.fields.len + LINK_LAYER_HEADER_SIZE),
                                            4, &radio_packet) > 0) {
            send_udp(&radio_packet, MAC_UNCODED_PACKET_SIZE);
        }
        send_udp(&radio_packet, MAC_UNCODED_PACKET_SIZE);
        read(feedback_fd, &metadata, sizeof(metadata));
        printf("Received event code: %d\n", metadata.event_code);
        while (metadata.event_code != 2) {
            read(feedback_fd, &metadata, sizeof(metadata));
            printf("Received event code: %d\n", metadata.event_code);
        }

        phy_tx_count += 12;
        printf("Total physical layer transmitter packets: %d\n", phy_tx_count);
        cnt++;
        printf("Packet %d sent!!\n", cnt);
        sleep(2);
    }
}

int main(int argc, char *argv[])
{
    pthread_t thread;
    char opt;
    int cnt = 0;
    if (argc != 2) {
        printf("Bad input sintax, specify ./prog_name opts...\n");
        return -1;
    }
    opt = argv[1][0];
    fd = socket_init(52000);
    feedback_fd = socket_init(52004);
    if (opt == 't') {           /* packet sending */
        transmitter_work();
    }else if (opt == 'r') {     /* packet receiving */
        receiver_work(NULL);
    }else if (opt == 'x') {     /* packet retransmit */
        init_chunk_handler(&send_packet_hchunk);
        while(cnt++ < RETRANSMIT_TEST_DURATION) {
            receive_packet();
            /* wait 1 second and retransmit */
            sleep(2);
            send_packet();
            printf("Sending packet: %d\r\n", cnt);
        }
    }else if (opt == 'w') {     /* packet send and wait */
        init_chunk_handler(&send_packet_hchunk);
        pthread_create(&thread, NULL, receiver_work, NULL);
        while(cnt++ < RETRANSMIT_TEST_DURATION) {
            printf("Sending packet: %d\r\n", cnt);
            send_packet();
            sleep(8);
        }
    }else {
        printf("Bad opt.\r\n");
    }
    close(fd);
    return 0;
}
