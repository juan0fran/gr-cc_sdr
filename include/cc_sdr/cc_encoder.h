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


#ifndef INCLUDED_CC_SDR_CC_ENCODER_H
#define INCLUDED_CC_SDR_CC_ENCODER_H

#include <cc_sdr/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace cc_sdr {

    /*!
     * \brief <+description of block+>
     * \ingroup cc_sdr
     *
     */
    class CC_SDR_API cc_encoder : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<cc_encoder> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of cc_sdr::cc_encoder.
       *
       * To avoid accidental use of raw pointers, cc_sdr::cc_encoder's
       * constructor is in a private implementation
       * class. cc_sdr::cc_encoder::make is the public interface for
       * creating new instances.
       */
      static sptr make(bool has_fec, bool has_rs, bool has_white, int plen);
    };

  } // namespace cc_sdr
} // namespace gr

#endif /* INCLUDED_CC_SDR_CC_ENCODER_H */

