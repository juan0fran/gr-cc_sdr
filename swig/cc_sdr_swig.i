/* -*- c++ -*- */

#define CC_SDR_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "cc_sdr_swig_doc.i"

%{
#include "cc_sdr/cc_decoder.h"
#include "cc_sdr/cc_encoder.h"
%}


%include "cc_sdr/cc_decoder.h"
GR_SWIG_BLOCK_MAGIC2(cc_sdr, cc_decoder);
%include "cc_sdr/cc_encoder.h"
GR_SWIG_BLOCK_MAGIC2(cc_sdr, cc_encoder);
