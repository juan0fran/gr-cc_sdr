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

#ifndef INCLUDED_CC_SDR_CC_ENCODER_IMPL_H
#define INCLUDED_CC_SDR_CC_ENCODER_IMPL_H

#include <cc_sdr/cc_encoder.h>

namespace gr {
  namespace cc_sdr {

    class cc_encoder_impl : public cc_encoder
    {
     private:
        bool d_has_fec;
        bool d_has_rs;
        bool d_has_white;

        int  d_plen;
        int  d_reclen;
        int  d_uncoded_len;
        int  d_coded_len;
        int  d_preamble_count;

        volatile bool d_carrier;

        bool carrier_sense(void);

     public:
      cc_encoder_impl(bool has_fec, bool has_rs, bool has_white, int plen);
      ~cc_encoder_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);

      void msg_handler(pmt::pmt_t pmt_msg);
    };
  } // namespace cc_sdr
} // namespace gr

#endif /* INCLUDED_CC_SDR_CC_ENCODER_IMPL_H */
