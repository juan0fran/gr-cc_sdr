/* -*- c++ -*- */
/*
 * Copyright 2016 <+YOU OR YOUR COMPANY+>.
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
#include "cc_decoder_impl.h"

extern "C" {
  #include "rs_work.h"
  #include "convolutional_coder.h"
}

#undef DEBUG
//#define DEBUG

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

#ifdef DEBUG
void
byte_err (int loc, unsigned char *dst)
{
        dst[loc-1] ^= (rand()%256);
}
#endif

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

cc_decoder::sptr
cc_decoder::make(bool has_fec, bool has_rs, bool has_white, int plen)
{
        return gnuradio::get_initial_sptr
                       (new cc_decoder_impl(has_fec, has_rs, has_white, plen));
}

/*
 * The private constructor
 */
cc_decoder_impl::cc_decoder_impl(bool has_fec, bool has_rs, bool has_white, int plen)
        : gr::block("cc_decoder",
                    gr::io_signature::make(0,0,0),
                    gr::io_signature::make(0,0,0))
{
        d_has_fec = has_fec;
        d_has_rs = has_rs;
        if (has_fec == false) {
                if (has_rs == true) {
                        d_uncoded_len = plen;
                        d_coded_len = plen + 32;
                        d_plen = d_coded_len;
                }else {
                        d_uncoded_len = plen;
                        d_coded_len = plen;
                        d_plen = plen + 2; /* packet + crc */
                }
        }else{
                if (has_rs == true) {
                        d_coded_len = plen + 32;
                        d_uncoded_len = d_coded_len - 32;
                        d_plen = d_coded_len*2 + 4; /* packet*2 + crc*2 + trellis header*2 */
                }else{
                        d_uncoded_len = plen;
                        d_coded_len = plen;
                        d_plen = plen*2 + 8; /* packet*2 + crc*2 + trellis header*2 */
                }
        }
        d_has_white = has_white;
        d_reclen = plen;

        d_correct = 0;
        d_correct_without_rs = 0;
        d_correct_with_rs = 0;
        d_incorrect = 0;

        std::cout << "[CC_DECODER]: Has FEC: " << d_has_fec << ". Has RS: " << d_has_rs << ". Has White: " << d_has_white << std::endl;
        std::cout << "[CC_DECODER]: Full packet len: " << d_plen << std::endl;
        std::cout << "[CC_DECODER]: Received packet len: " << d_reclen << std::endl;
        std::cout << "[CC_DECODER]: RS Encoded len: " << d_coded_len << ". RS Decoded len: " << d_uncoded_len << std::endl;
        message_port_register_out(pmt::mp("out"));
        message_port_register_in(pmt::mp("in"));
        set_msg_handler(pmt::mp("in"),
                        boost::bind(&cc_decoder_impl::msg_handler, this, _1));
}

/*
 * Our virtual destructor.
 */
cc_decoder_impl::~cc_decoder_impl()
{
}

void
cc_decoder_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
}

int
cc_decoder_impl::general_work (int noutput_items,
                               gr_vector_int &ninput_items,
                               gr_vector_const_void_star &input_items,
                               gr_vector_void_star &output_items)
{
        return 0;
}

void
cc_decoder_impl::msg_handler(pmt::pmt_t pmt_msg)
{
        pmt::pmt_t msg = pmt::cdr(pmt_msg);

        unsigned short checksum;
        unsigned short nBytes;
        unsigned short c;
        int corrected;

        uint8_t data[d_plen];
        uint8_t rxBuffer[4];
        uint8_t rxPacket[d_plen]; /* plus the CRC */
        uint8_t uncodedPacket[d_uncoded_len];

        size_t offset(0);

        memcpy(data, pmt::uniform_vector_elements(msg, offset), sizeof(data));

        if (d_has_fec == true) {
                uint8_t *pDecData = rxPacket;
                /* initialize fecDecoder */
                fecDecode(NULL, NULL, 0);
                /* Set variables */
                c = 0;
                nBytes = d_plen;
                /* Initialize the decoder */
                while (nBytes > 0) {
                        unsigned short nBytesOut;
                        // Read 4 bytes from the RXFIFO and store them in rxBuffer
                        rxBuffer[0] = data[c+0];
                        rxBuffer[1] = data[c+1];
                        rxBuffer[2] = data[c+2];
                        rxBuffer[3] = data[c+3];
                        nBytesOut = fecDecode(pDecData, rxBuffer, nBytes);
                        nBytes -= nBytesOut;
                        pDecData += nBytesOut;
                        c = c + 4;
                }
        }else {
            memcpy(rxPacket, data, d_coded_len);
        }
        if (d_has_white == true) {
                xor_pn9(rxPacket, d_coded_len);
        }

        if (d_has_rs == true) {
                if ( (corrected = decode_rs_message(rxPacket, d_coded_len, uncodedPacket, d_uncoded_len)) != -1) {
                        /* if decode true */
                        d_correct += 1;
                        d_correct_with_rs += 1;
                        std::cout << "[CC_DECODER]: Ok decoded: " << d_correct << " With RS: " << d_correct_with_rs << " Without RS: " << d_correct_without_rs << " Incorrect: " << d_incorrect << " corrected bytes: "<< corrected << " Timer: " << time(NULL) << std::endl;
                        message_port_pub(pmt::mp("out"),
                                         pmt::cons(pmt::PMT_NIL,
                                                   pmt::init_u8vector(d_uncoded_len, uncodedPacket)));
                }else{
                        d_incorrect += 1;
                        std::cout << "[CC_DECODER]: Ok decoded: " << d_correct << " With RS: " << d_correct_with_rs << " Without RS: " << d_correct_without_rs << " Incorrect: " << d_incorrect << " Timer: " << time(NULL) << std::endl;
                }
        }else{
                /* rx packet already has the decoded data from the FEC */
                /* Perform checksum */
                checksum = 0xFFFF;
                // Init value for CRC calculation
                /* Go from d_reclen+2, which is the received length plus the CRC */
                for (int i = 0; i < (d_reclen + 2); i++) {
                        checksum = calcCRC(rxPacket[i], checksum);
                }
                /* if the previous calculous is 0, then CRC ok */
                if (!checksum) {
                        d_correct += 1;
                        std::cout << "[CC_DECODER]: Ok decoded: " << d_correct << " Timer: " << time(NULL) << std::endl;
                        message_port_pub(pmt::mp("out"),
                                         pmt::cons(pmt::PMT_NIL,
                                                   pmt::init_u8vector(d_uncoded_len, rxPacket)));
                }
        }
}
}   /* namespace cc_sdr */
} /* namespace gr */
