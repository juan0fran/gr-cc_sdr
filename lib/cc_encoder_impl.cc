/* -*- c++ -*- */
/*
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <time.h>
#include <cstdio>

#include <gnuradio/io_signature.h>
#include "cc_encoder_impl.h"


extern "C" {
  #include "rs_work.h"
  #include "convolutional_coder.h"
}

#define SYNC_WORD_ENCODER_W0    0xD2
#define SYNC_WORD_ENCODER_W1    0x59

static const uint8_t pn9_table[] = {
        0xff, 0xe1, 0x1d, 0x9a, 0xed, 0x85, 0x33, 0x24,
        0xea, 0x7a, 0xd2, 0x39, 0x70, 0x97, 0x57, 0x0a,
        0x54, 0x7d, 0x2d, 0xd8, 0x6d, 0x0d, 0xba, 0x8f,
        0x67, 0x59, 0xc7, 0xa2, 0xbf, 0x34, 0xca, 0x18,
        0x30, 0x53, 0x93, 0xdf, 0x92, 0xec, 0xa7, 0x15,
        0x8a, 0xdc, 0xf4, 0x86, 0x55, 0x4e, 0x18, 0x21,
        0x40, 0xc4, 0xc4, 0xd5, 0xc6, 0x91, 0x8a, 0xcd,
        0xe7, 0xd1, 0x4e, 0x09, 0x32, 0x17, 0xdf, 0x83,
        0xff, 0xf0, 0x0e, 0xcd, 0xf6, 0xc2, 0x19, 0x12,
        0x75, 0x3d, 0xe9, 0x1c, 0xb8, 0xcb, 0x2b, 0x05,
        0xaa, 0xbe, 0x16, 0xec, 0xb6, 0x06, 0xdd, 0xc7,
        0xb3, 0xac, 0x63, 0xd1, 0x5f, 0x1a, 0x65, 0x0c,
        0x98, 0xa9, 0xc9, 0x6f, 0x49, 0xf6, 0xd3, 0x0a,
        0x45, 0x6e, 0x7a, 0xc3, 0x2a, 0x27, 0x8c, 0x10,
        0x20, 0x62, 0xe2, 0x6a, 0xe3, 0x48, 0xc5, 0xe6,
        0xf3, 0x68, 0xa7, 0x04, 0x99, 0x8b, 0xef, 0xc1,
        0x7f, 0x78, 0x87, 0x66, 0x7b, 0xe1, 0x0c, 0x89,
        0xba, 0x9e, 0x74, 0x0e, 0xdc, 0xe5, 0x95, 0x02,
        0x55, 0x5f, 0x0b, 0x76, 0x5b, 0x83, 0xee, 0xe3,
        0x59, 0xd6, 0xb1, 0xe8, 0x2f, 0x8d, 0x32, 0x06,
        0xcc, 0xd4, 0xe4, 0xb7, 0x24, 0xfb, 0x69, 0x85,
        0x22, 0x37, 0xbd, 0x61, 0x95, 0x13, 0x46, 0x08,
        0x10, 0x31, 0x71, 0xb5, 0x71, 0xa4, 0x62, 0xf3,
        0x79, 0xb4, 0x53, 0x82, 0xcc, 0xc5, 0xf7, 0xe0,
        0x3f, 0xbc, 0x43, 0xb3, 0xbd, 0x70, 0x86, 0x44,
        0x5d, 0x4f, 0x3a, 0x07, 0xee, 0xf2, 0x4a, 0x81,
        0xaa, 0xaf, 0x05, 0xbb, 0xad, 0x41, 0xf7, 0xf1,
        0x2c, 0xeb, 0x58, 0xf4, 0x97, 0x46, 0x19, 0x03,
        0x66, 0x6a, 0xf2, 0x5b, 0x92, 0xfd, 0xb4, 0x42,
        0x91, 0x9b, 0xde, 0xb0, 0xca, 0x09, 0x23, 0x04,
        0x88, 0x98, 0xb8, 0xda, 0x38, 0x52, 0xb1, 0xf9,
        0x3c, 0xda, 0x29, 0x41, 0xe6, 0xe2, 0x7b, 0xf0,
        0x1f, 0xde, 0xa1
};

static int xor_pn9(uint8_t *buf, size_t len)
{
        if (len > sizeof(pn9_table))
                return -1;

        for (int i = 0; i < len; ++i)
                buf[i] ^= pn9_table[i];

        return 0;
}

namespace gr {
namespace cc_sdr {

cc_encoder::sptr
cc_encoder::make(bool has_fec, bool has_rs, bool has_white, int plen)
{
        return gnuradio::get_initial_sptr
                       (new cc_encoder_impl(has_fec, has_rs, has_white, plen));
}

/*
 * The private constructor
 */
cc_encoder_impl::cc_encoder_impl(bool has_fec, bool has_rs, bool has_white, int plen)
        : gr::sync_block("cc_encoder",
                         gr::io_signature::make(0,0,0),
                         gr::io_signature::make(0,0,0))
{
        d_carrier = false;
        d_preamble_count = 12;
        d_has_fec = has_fec;
        d_has_rs = has_rs;
        if (has_fec == false) {
                if (has_rs == true) {
                        d_uncoded_len = plen;
                        d_coded_len = plen + 32;
                        d_plen = d_coded_len;
                }else {
                        d_uncoded_len = plen;
                        d_coded_len = plen + 2;
                        d_plen = plen + 2; /* packet + crc */
                }
        }else{
                if (has_rs == true) {
                        d_coded_len = plen + 32;
                        d_uncoded_len = d_coded_len - 32;
                        d_plen = d_coded_len*2 + 4; /* packet*2 + crc*2 + trellis header*2 */
                }else{
                        d_uncoded_len = plen;
                        d_coded_len = plen + 2;
                        d_plen = plen*2 + 8; /* packet*2 + crc*2 + trellis header*2 */
                }
        }
        d_has_white = has_white;
        d_reclen = plen;

        std::cout << "[CC_ENCODER]: Has FEC: " << d_has_fec << ". Has RS: " << d_has_rs << ". Has White: " << d_has_white << std::endl;
        std::cout << "[CC_ENCODER]: Full packet len: " << d_plen << std::endl;
        std::cout << "[CC_ENCODER]: Received packet len: " << d_reclen << std::endl;
        std::cout << "[CC_ENCODER]: RS Encoded len: " << d_coded_len << ". RS Decoded len: " << d_uncoded_len << std::endl;

        message_port_register_out(pmt::mp("out"));
        message_port_register_in(pmt::mp("in"));
        set_msg_handler(pmt::mp("in"),
                        boost::bind(&cc_encoder_impl::msg_handler, this, _1));
}

/*
 * Our virtual destructor.
 */
cc_encoder_impl::~cc_encoder_impl()
{
}

int
cc_encoder_impl::work(int noutput_items,
                      gr_vector_const_void_star &input_items,
                      gr_vector_void_star &output_items)
{
        return 0;
}
void
cc_encoder_impl::msg_handler(pmt::pmt_t pmt_msg)
{
        pmt::pmt_t msg = pmt::cdr(pmt_msg);
        unsigned int msg_size = pmt::length(msg);

        uint8_t data_in[d_uncoded_len];
        uint8_t data_rs_out[d_coded_len + d_preamble_count + 4];
        uint8_t data_out[d_plen + d_preamble_count + 4];

        int corrected;
        unsigned short checksum;

        size_t offset(0);

        if (msg_size > d_uncoded_len) {
                std::printf("Error occured, length of packet is too long\n");
                return;
        }

        /* paddind is automatic as data_in is initialised to 0 */
        for (int i = 0; i < d_uncoded_len; i++)
                data_in[i] = 0x00;

        memcpy(data_in, pmt::uniform_vector_elements(msg, offset), msg_size);
        std::printf("[CC_ENCODER]: Message size: %d\n", msg_size);

        if ((msg_size == 1) && (data_in[0] == 0xAA)) {
            std::printf("[CC_ENCODER]: Sending 250ms preamble\n");
            for (int i = 0; i < (d_plen + d_preamble_count + 4); i++)
                data_out[i] = 0xAA;
                message_port_pub(pmt::mp("out"),
                                pmt::cons(pmt::PMT_NIL,
                                    pmt::init_u8vector(d_plen + d_preamble_count + 4, data_out)));
                return;
        }
        if (d_has_rs == true) {
                if (encode_rs_message(data_in, d_uncoded_len, data_rs_out, d_coded_len) != d_coded_len) {
                        std::printf("[CC_ENCODER]: Encode RS failed\n");
                        return;
                }
        }else {
                memcpy(data_rs_out, data_in, d_uncoded_len);
                checksum = 0xFFFF;
                // Init value for CRC calculation
                /* Go from d_reclen+2, which is the received length plus the CRC */
                for (int i = 0; i < d_uncoded_len; i++) {
                        checksum = calcCRC(data_rs_out[i], checksum);
                }
                data_rs_out[d_uncoded_len] = checksum >> 8;
                data_rs_out[d_uncoded_len + 1] = checksum & 0x00FF;
        }
        if (d_has_white)
                xor_pn9(data_rs_out, d_coded_len);

        if (d_has_fec) {
                if (fecEncode(data_rs_out, data_out, d_coded_len) != d_plen) {
                        std::printf("[CC_ENCODER]: Sizes of RS or FEC are not consistent\n");
                        return;
                }
        }else{
                memcpy(data_out, data_rs_out, d_coded_len);
        }

        for (int i = d_plen; i >= 0; i--)
                data_out[i + d_preamble_count] = data_out[i];

        int idx = 0;

        for (idx = 0; idx < d_preamble_count - 4; idx++) {
                data_out[idx] = 0xAA;
        }

        data_out[idx++] = SYNC_WORD_ENCODER_W0;
        data_out[idx++] = SYNC_WORD_ENCODER_W1;
        data_out[idx++] = SYNC_WORD_ENCODER_W0;
        data_out[idx++] = SYNC_WORD_ENCODER_W1;

        data_out[d_plen + d_preamble_count + 1] = 0xAA;
        data_out[d_plen + d_preamble_count + 2] = 0xAA;

        message_port_pub(pmt::mp("out"),
                         pmt::cons(pmt::PMT_NIL,
                                   pmt::init_u8vector(d_plen + d_preamble_count + 4, data_out)));
}

bool
cc_encoder_impl::carrier_sense(void)
{
        return d_carrier;
}
}   /* namespace cc_sdr */
} /* namespace gr */
