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

#include <cstdio>

#include <gnuradio/io_signature.h>
#include "cc_decoder_impl.h"

extern "C" {
  #include "rs_work.h"
  #include "trellis_decoder.h"
}

#undef DEBUG

#ifdef DEBUG
void
byte_err (int loc, unsigned char *dst)
{
  dst[loc-1] ^= (rand()%256);
}
#endif

namespace gr {
  namespace cc_sdr {

    cc_decoder::sptr
    cc_decoder::make(bool has_fec, bool has_rs, int plen)
    {
      return gnuradio::get_initial_sptr
        (new cc_decoder_impl(has_fec, has_rs, plen));
    }

    /*
     * The private constructor
     */
    cc_decoder_impl::cc_decoder_impl(bool has_fec, bool has_rs, int plen)
      : gr::block("cc_decoder",
              gr::io_signature::make(0,0,0),
              gr::io_signature::make(0,0,0))
    {
      d_has_fec = has_fec;
      d_has_rs = has_rs;
      if (has_fec == false){
        d_plen = plen + 2; /* packet + crc */
      }else{
        if (has_rs == true){
          d_plen = plen*2 + 8; /* packet*2 + trellis header*2 */
          d_coded_len = plen;
          d_uncoded_len = d_coded_len - 32;
        }else{
          d_plen = plen*2 + 8; /* packet*2 + crc*2 + trellis header*2 */
        }
      }
      d_reclen = plen;

      std::cout << "Has FEC: " << has_fec << ". Has RS: " << has_rs << std::endl;
      std::cout << "Full packet len: " << d_plen << std::endl;
      std::cout << "Received packet len: " << d_reclen << std::endl;
      std::cout << "RS Encoded len: " << d_coded_len << ". RS Decoded len: " << d_uncoded_len << std::endl;
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
      
      uint8_t data[d_plen];
      
      size_t offset(0);
      
      unsigned short checksum;
      unsigned short nBytes;
      unsigned short c;

      uint8_t rxBuffer[4];
      uint8_t rxPacket[d_reclen + 2]; /* plus the CRC */
      uint8_t uncodedPacket[d_uncoded_len];

      memcpy(data, pmt::uniform_vector_elements(msg, offset), sizeof(data));
    
      #ifdef DEBUG
      std::cout << "Received data:" << std::endl;
      for(int i = 0; i < d_plen; i++)
        std::printf ("0x%02X%s" , data[i], (i % 16 == 15) ? "\n" : " ");
      std::cout << std::endl;
      #endif 
      
      /* introduce errors to DATA */
      #ifdef DEBUG
      byte_err(rand()%255, data);
      byte_err(rand()%255, data);
      byte_err(rand()%255, data);
      byte_err(rand()%255, data);
      byte_err(rand()%255, data);
      byte_err(rand()%255, data);
      byte_err(rand()%255, data);
      byte_err(rand()%255, data);
      byte_err(rand()%255, data);
      byte_err(rand()%255, data);
      byte_err(rand()%255, data);
      byte_err(rand()%255, data);
      byte_err(rand()%255, data);
      byte_err(rand()%255, data);
      #endif

      #ifdef DEBUG
      std::cout << "Modified data:" << std::endl;
      for(int i = 0; i < d_plen; i++)
        std::printf ("0x%02X%s" , data[i], (i % 16 == 15) ? "\n" : " ");
      std::cout << std::endl;
      #endif 

      if (d_has_fec == true){
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
        #ifdef DEBUG
        std::cout << "Decoded data:" << std::endl;
        for(int i = 0; i < d_reclen; i++)
          std::printf("0x%02X%s" , rxPacket[i], (i % 16 == 15) ? "\n" : " ");
        std::cout << std::endl;
        #endif 
        
        if (d_has_rs == true){
          std::cout << "Goin to check CRC from received packet" << std::endl;
          /* rx packet already has the decoded data from the FEC */
          /* Perform checksum */
          checksum = 0xFFFF;
          // Init value for CRC calculation
          /* Go from d_reclen+2, which is the received length plus the CRC */
          for (int i = 0; i < (d_coded_len + 2); i++){
              checksum = calcCRC(rxPacket[i], checksum);
          }
          /* if the previous calculous is 0, then CRC ok */
          if (!checksum){
            std::cout << "CRC OK" << std::endl;
            message_port_pub(pmt::mp("out"),
              pmt::cons(pmt::PMT_NIL,
              pmt::init_u8vector(d_uncoded_len, rxPacket)));
          }else{
              std::cout << "CRC NO OK, trying to RS decode" << std::endl;              
              /* decode rs */
              if (decode_rs_message(rxPacket, d_coded_len, uncodedPacket, d_uncoded_len) == d_uncoded_len){
                /* if decode true */
                std::cout << "RS Decode went OK!!" << std::endl;
                message_port_pub(pmt::mp("out"),
                    pmt::cons(pmt::PMT_NIL,
                    pmt::init_u8vector(d_uncoded_len, uncodedPacket)));
              }else{
                std::cout << "Packet recovery failed, broken packet!!" << std::endl;
              }
          }
        }else{
          std::cout << "Goin to check CRC from received packet" << std::endl;
          /* rx packet already has the decoded data from the FEC */
          /* Perform checksum */
          checksum = 0xFFFF;
          // Init value for CRC calculation
          /* Go from d_reclen+2, which is the received length plus the CRC */
          for (int i = 0; i < (d_reclen + 2); i++){
              checksum = calcCRC(rxPacket[i], checksum);
          }
          /* if the previous calculous is 0, then CRC ok */
          if (!checksum){
            std::cout << "CRC OK" << std::endl;
            message_port_pub(pmt::mp("out"),
              pmt::cons(pmt::PMT_NIL,
              pmt::init_u8vector(d_reclen, rxPacket)));
          }
        }
      }else{
        /* copy all packet data + crc */
        memcpy(rxPacket, data, d_reclen+2);
        /* Perform checksum */
        checksum = 0xFFFF;
        // Init value for CRC calculation
        /* Go from d_reclen+2, which is the received length plus the CRC */
        for (int i = 0; i < (d_reclen + 2); i++){
            checksum = calcCRC(rxPacket[i], checksum);
        }
        /* if the previous calculous is 0, then CRC ok */
        if (!checksum){
          std::cout << "CRC OK" << std::endl;
          message_port_pub(pmt::mp("out"),
            pmt::cons(pmt::PMT_NIL,
            pmt::init_u8vector(d_reclen, rxPacket)));
        }
      }
    }
  } /* namespace cc_sdr */
} /* namespace gr */

