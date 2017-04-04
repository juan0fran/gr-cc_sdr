#include "socket_utils.h"
#include "link_layer.h"
#include "of_reed-solomon_gf_2_m.h"

#define __OF_TEST__

int fd;

void * write_socket(void * args)
{
    unsigned int i = 0;
    char buffer[128];
    printf("Write socket task\n");
    while(1) {
        sprintf(buffer, "Test from USRP %d", i++);
        //write(fd, buffer, strlen(buffer));
        usleep((rand()%500 + 500) * 1000);
    }
}

#ifndef __OF_TEST__

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

static of_rs_2_m_cb_t rs;

void print_ptr(uint8_t * ptr)
{
    int j;
    for (j = 0; j < rs.encoding_symbol_length; j++)
        printf("%02x%s", ptr[j], j%32==31?"\n":" ");
    printf("\n");
}

void test_openfec(void)
{

    of_rs_2_m_parameters_t parms;

    void * enc_sym_tabs[OF_MAX_ENCODING_SYMBOLS];
    void * dec_sym_tabs[OF_MAX_ENCODING_SYMBOLS];

    uint8_t symb_reserved_space_tx[OF_MAX_ENCODING_SYMBOLS * OF_MAX_SYMBOL_SIZE];
    uint8_t symb_reserved_space_rx[OF_MAX_ENCODING_SYMBOLS * OF_MAX_SYMBOL_SIZE];

    int i;

    of_rs_2_m_create_codec_instance(&rs);
    parms.encoding_symbol_length = 100;
    parms.nb_source_symbols = 6;
    parms.nb_repair_symbols = 2;

    of_rs_2_m_set_fec_parameters(&rs, &parms);

    memset(enc_sym_tabs, 0, sizeof(enc_sym_tabs));
    memset(dec_sym_tabs, 0, sizeof(dec_sym_tabs));

    for (i = 0; i < rs.nb_source_symbols; i++){
        memset(&symb_reserved_space_tx[i * rs.encoding_symbol_length], 0xAA, rs.encoding_symbol_length);
        enc_sym_tabs[i] = &symb_reserved_space_tx[i * rs.encoding_symbol_length];
        //of_rs_2_m_build_repair_symbol(&rs, enc_sym_tabs, i);
        //llc.esi = i;
        //build_llc_packet(enc_sym_tabs[i], rs.encoding_symbol_length, &llc, &packet);
        //radio_send_packet(&spi, &radio, &packet);
        printf("Encoded %d\n", i);
        print_ptr(enc_sym_tabs[i]); 
    }

    for (i = rs.nb_source_symbols; i < rs.nb_encoding_symbols; i++){
        memset(&symb_reserved_space_tx[i * rs.encoding_symbol_length], 0xAA, rs.encoding_symbol_length);
        enc_sym_tabs[i] = &symb_reserved_space_tx[i * rs.encoding_symbol_length];
        of_rs_2_m_build_repair_symbol(&rs, enc_sym_tabs, i);

        printf("Encoded %d\n", i);
        print_ptr(enc_sym_tabs[i]); 
        //llc.esi = i;
        //build_llc_packet(enc_sym_tabs[i], rs.encoding_symbol_length, &llc, &packet);
        //radio_send_packet(&spi, &radio, &packet);
    }
    printf("\n");
    of_rs_2_m_set_fec_parameters(&rs, &parms);

    for (i = 0; i < rs.nb_source_symbols; i++){
        memcpy(&symb_reserved_space_rx[(i+1)*rs.encoding_symbol_length], enc_sym_tabs[i+1], rs.encoding_symbol_length);     
        printf ("Decode w new %d\n", of_rs_2_m_decode_with_new_symbol(&rs, &symb_reserved_space_rx[(i+1) * rs.encoding_symbol_length], i+1));
    }
    of_rs_2_m_get_source_symbols_tab(&rs, dec_sym_tabs);
    int equal = 0;
    int j;
    for (i = 0; i < rs.nb_source_symbols; i++){

        printf("Decoded %d\n", i);
        print_ptr(dec_sym_tabs[i]);
        if (memcmp(dec_sym_tabs[i], enc_sym_tabs[i], rs.encoding_symbol_length) == 0){
            equal++;
        }
    }
    if (equal == rs.nb_source_symbols){
        printf("Correct\n");
    }else{
        printf("Incorrect\n");
    }
}

void * read_socket(void * args)
{
    radio_packet_t packet;
    uint8_t buffer[256];
    int ret;
    printf("Read socket task\n");
    llc_parms_t llc;

    of_rs_2_m_cb_t rs;
    of_rs_2_m_parameters_t parms;

    void * enc_sym_tabs[OF_MAX_ENCODING_SYMBOLS];
    void * dec_sym_tabs[OF_MAX_ENCODING_SYMBOLS];

    uint8_t symb_reserved_space_tx[OF_MAX_ENCODING_SYMBOLS * OF_MAX_SYMBOL_SIZE];
    uint8_t symb_reserved_space_rx[OF_MAX_ENCODING_SYMBOLS * OF_MAX_SYMBOL_SIZE];

    of_rs_2_m_create_codec_instance(&rs);

    uint8_t current_packet = 0;
    bool decoded_done = false;
    uint32_t cnt = 0;

    int i, j;

    chunk_handler_t chunk_tx, chunk_rx;

    init_chunk_handler(&chunk_tx);
    init_chunk_handler(&chunk_rx);
    memset(symb_reserved_space_tx, 0xAA, sizeof(symb_reserved_space_tx));
    

    while(1){
        if ( (ret = read(fd, buffer, MAC_UNCODED_PACKET_SIZE) ) > 0){
            memcpy(packet.raw, buffer, ret);
            ret = set_new_packet_to_chunk(&chunk_rx, &packet, symb_reserved_space_rx);
            if (ret > 0){
                cnt++;
                printf("Received a whole chunk: %d received until now\n", cnt);
                for (i = 0, j = 0; i < ret; i++){
                    if (symb_reserved_space_rx[i] == i%256){
                        j++;
                    }
                }
                printf("Size of chunk is: %d and contains %d corrects\n", ret, j);

                /* Sending! */
                /*
                sleep(1);
                printf("Slepts and sending more!\n");
                while ( (ret = get_new_packet_from_chunk(&chunk_tx, symb_reserved_space_rx, ret, 2, &packet) ) >= 0){
                    memcpy(buffer, packet.raw, sizeof(packet.raw));
                    write(fd, buffer, MAC_UNCODED_PACKET_SIZE);
                    if (ret == 0)
                        break;
                }*/

            }
        }else{
            return NULL;
        }
    }


    while(1){
        if ( (ret = read(fd, buffer, MAC_UNCODED_PACKET_SIZE) ) > 0){
            memcpy(packet.raw, buffer, ret);
            get_llc_packet(&packet, &llc);
            if (current_packet != llc.chunk_seq){
                printf("[NEW]: New chunk sequence: %d\n", llc.chunk_seq);
                parms.nb_repair_symbols = llc.k;
                parms.nb_source_symbols = llc.r;
                of_rs_2_m_set_fec_parameters(&rs, &parms);
                current_packet = llc.chunk_seq;
                decoded_done = false;
                cnt = 0;
            }
            if (!decoded_done){
                printf("[NEW]: K: %d, R: %d and ESI: %d\n", llc.k, llc.r, llc.esi);
                memcpy(&symb_reserved_space_rx[cnt * rs.encoding_symbol_length], packet.fields.data, rs.encoding_symbol_length);
                of_rs_2_m_decode_with_new_symbol(&rs, &symb_reserved_space_rx[cnt * rs.encoding_symbol_length], llc.esi);
                cnt++;
                if (cnt == llc.k && rs.decoding_finished){
                    decoded_done = true;
                    ret = of_rs_2_m_get_source_symbols_tab(&rs, dec_sym_tabs);
                    printf("[OPENFEC] get source symbols returned: %d\n", ret);
                }
            }else{
                printf("[SKIPPED]: K: %d, R: %d and ESI: %d\n", llc.k, llc.r, llc.esi);
            }
        }
    }
}

#endif

int main(void)
{
    pthread_t threads[2];
    int rc;
    fd = socket_init(52001);

    #ifndef __OF_TEST__
    pthread_create(&threads[0], NULL, write_socket, NULL);
    pthread_create(&threads[1], NULL, read_socket, NULL);
    #else
    read_socket(NULL);
    //test_openfec();
    #endif

    //while(1);
}