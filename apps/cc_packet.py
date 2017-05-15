#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Cc Packet
# Generated: Fri Apr 28 17:10:28 2017
##################################################

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"

from PyQt4 import Qt
from gnuradio import blocks
from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio import qtgui
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.qtgui import Range, RangeWidget
from optparse import OptionParser
import cc_sdr
import numpy
import sip
import sys
import time


class cc_packet(gr.top_block, Qt.QWidget):

    def __init__(self, filter_width=96e3, rate=9600):
        gr.top_block.__init__(self, "Cc Packet")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Cc Packet")
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "cc_packet")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())

        ##################################################
        # Parameters
        ##################################################
        self.filter_width = filter_width
        self.rate = rate

        ##################################################
        # Variables
        ##################################################
        self.threshold = threshold = 3
        self.sps = sps = 5
        self.source_rate = source_rate = 480e3
        self.samp_rate = samp_rate = 48e3
        self.preamble = preamble = '101010101010101010101010101010'
        self.packet_length = packet_length = 255
        self.offset_rx = offset_rx = 0
        self.freq_comp_rate = freq_comp_rate = 96e3
        self.fec = fec = 0
        self.access_code = access_code = '11010011100100011101001110010001'

        ##################################################
        # Blocks
        ##################################################
        self._offset_rx_range = Range(-100e3, 100e3, 1e3, 0, 200)
        self._offset_rx_win = RangeWidget(self._offset_rx_range, self.set_offset_rx, "Offset RX", "counter_slider", float)
        self.top_layout.addWidget(self._offset_rx_win)
        self.uhd_usrp_source_0 = uhd.usrp_source(
        	",".join(("", "")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(1),
        	),
        )
        self.uhd_usrp_source_0.set_samp_rate(source_rate)
        self.uhd_usrp_source_0.set_center_freq(433.92e6 - 50e3, 0)
        self.uhd_usrp_source_0.set_gain(65, 0)
        self.uhd_usrp_source_0.set_antenna("TX/RX", 0)
        self.uhd_usrp_sink_0 = uhd.usrp_sink(
        	",".join(("", "")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(1),
        	),
        	"packet_len",
        )
        self.uhd_usrp_sink_0.set_samp_rate(source_rate)
        self.uhd_usrp_sink_0.set_center_freq(434.92*1e6, 0)
        self.uhd_usrp_sink_0.set_gain(76, 0)
        self.uhd_usrp_sink_0.set_antenna("TX/RX", 0)
        self.rational_resampler_xxx_0_0 = filter.rational_resampler_ccc(
                interpolation=int(freq_comp_rate),
                decimation=int(source_rate),
                taps=None,
                fractional_bw=None,
        )
        self.rational_resampler_xxx_0 = filter.rational_resampler_ccc(
                interpolation=10,
                decimation=1,
                taps=None,
                fractional_bw=None,
        )
        self.qtgui_sink_x_0_0 = qtgui.sink_c(
        	1024, #fftsize
        	firdes.WIN_BLACKMAN_hARRIS, #wintype
        	0, #fc
        	source_rate, #bw
        	"", #name
        	True, #plotfreq
        	True, #plotwaterfall
        	True, #plottime
        	True, #plotconst
        )
        self.qtgui_sink_x_0_0.set_update_time(1.0/10)
        self._qtgui_sink_x_0_0_win = sip.wrapinstance(self.qtgui_sink_x_0_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_sink_x_0_0_win)
        
        self.qtgui_sink_x_0_0.enable_rf_freq(False)
        
        
          
        self.qtgui_sink_x_0 = qtgui.sink_c(
        	1024, #fftsize
        	firdes.WIN_BLACKMAN_hARRIS, #wintype
        	0, #fc
        	samp_rate, #bw
        	"", #name
        	True, #plotfreq
        	True, #plotwaterfall
        	True, #plottime
        	True, #plotconst
        )
        self.qtgui_sink_x_0.set_update_time(1.0/10)
        self._qtgui_sink_x_0_win = sip.wrapinstance(self.qtgui_sink_x_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_sink_x_0_win)
        
        self.qtgui_sink_x_0.enable_rf_freq(False)
        
        
          
        self.qtgui_number_sink_0 = qtgui.number_sink(
            gr.sizeof_float,
            0,
            qtgui.NUM_GRAPH_HORIZ,
            2
        )
        self.qtgui_number_sink_0.set_update_time(0.10)
        self.qtgui_number_sink_0.set_title("")
        
        labels = ["Received Power", "Normalized Power", "", "", "",
                  "", "", "", "", ""]
        units = ["dBm", "dBm/Hz", "", "", "",
                 "", "", "", "", ""]
        colors = [("blue", "red"), ("blue", "red"), ("black", "black"), ("black", "black"), ("black", "black"),
                  ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black")]
        factor = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        for i in xrange(2):
            self.qtgui_number_sink_0.set_min(i, -174)
            self.qtgui_number_sink_0.set_max(i, 10)
            self.qtgui_number_sink_0.set_color(i, colors[i][0], colors[i][1])
            if len(labels[i]) == 0:
                self.qtgui_number_sink_0.set_label(i, "Data {0}".format(i))
            else:
                self.qtgui_number_sink_0.set_label(i, labels[i])
            self.qtgui_number_sink_0.set_unit(i, units[i])
            self.qtgui_number_sink_0.set_factor(i, factor[i])
        
        self.qtgui_number_sink_0.enable_autoscale(False)
        self._qtgui_number_sink_0_win = sip.wrapinstance(self.qtgui_number_sink_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_number_sink_0_win)
        self.low_pass_filter_0_0 = filter.fir_filter_ccf(1, firdes.low_pass(
        	1, source_rate, samp_rate/2.0, samp_rate/20.0, firdes.WIN_HAMMING, 6.76))
        self.low_pass_filter_0 = filter.fir_filter_ccf(1, firdes.low_pass(
        	1, samp_rate, rate * 1.0, 1e3, firdes.WIN_HAMMING, 6.76))
        self.freq_xlating_fir_filter_xxx_1 = filter.freq_xlating_fir_filter_ccc(int(freq_comp_rate/samp_rate), (firdes.low_pass(1, freq_comp_rate, rate*0.625, rate/20.0)), 0, freq_comp_rate)
        self.freq_xlating_fir_filter_xxx_0 = filter.freq_xlating_fir_filter_ccc(1, (firdes.low_pass(1, source_rate, filter_width/2.0, filter_width/20.0)), 50e3 - offset_rx, source_rate)
        self.digital_gfsk_mod_1 = digital.gfsk_mod(
        	samples_per_symbol=5,
        	sensitivity=0.5,
        	bt=0.5,
        	verbose=False,
        	log=False,
        )
        self.digital_gfsk_demod_0 = digital.gfsk_demod(
        	samples_per_symbol=5,
        	sensitivity=0.5,
        	gain_mu=0.175,
        	mu=0.5,
        	omega_relative_limit=0.005,
        	freq_error=0.0,
        	verbose=False,
        	log=False,
        )
        self.digital_fll_band_edge_cc_0 = digital.fll_band_edge_cc(freq_comp_rate / rate, 0.35, 25, 0.01)
        self.digital_correlate_access_code_tag_bb_0_0 = digital.correlate_access_code_tag_bb(access_code, threshold, "syncword")
        self.cc_sdr_fixedlen_packet_synchronizer_0 = cc_sdr.fixedlen_packet_synchronizer("syncword", "packet_len", (packet_length +  fec*(packet_length + 8)) * 8, numpy.byte)
        self.cc_sdr_cc_encoder_0 = cc_sdr.cc_encoder(False, True, True, 255)
        self.cc_sdr_cc_decoder_0 = cc_sdr.cc_decoder(False, True, True, packet_length)
        self.blocks_unpacked_to_packed_xx_0_0 = blocks.unpacked_to_packed_bb(1, gr.GR_MSB_FIRST)
        self.blocks_tagged_stream_to_pdu_0_0 = blocks.tagged_stream_to_pdu(blocks.byte_t, "packet_len")
        self.blocks_tagged_stream_multiply_length_0_0 = blocks.tagged_stream_multiply_length(gr.sizeof_char*1, "packet_len", 1/8.0)
        self.blocks_tagged_stream_multiply_length_0 = blocks.tagged_stream_multiply_length(gr.sizeof_char*1, "packet_len", 8 * 5 * 10)
        self.blocks_socket_pdu_0_0 = blocks.socket_pdu("TCP_SERVER", "", "52001", 223, False)
        self.blocks_pdu_to_tagged_stream_0 = blocks.pdu_to_tagged_stream(blocks.byte_t, "packet_len")
        self.blocks_nlog10_ff_0_0 = blocks.nlog10_ff(10, 1, -56-40.792)
        self.blocks_nlog10_ff_0 = blocks.nlog10_ff(10, 1, -56)
        self.blocks_multiply_conjugate_cc_0 = blocks.multiply_conjugate_cc(1)
        self.blocks_moving_average_xx_0 = blocks.moving_average_ff(4000, 1/4000.0, 4000)
        self.blocks_complex_to_real_0 = blocks.complex_to_real(1)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.blocks_socket_pdu_0_0, 'pdus'), (self.cc_sdr_cc_encoder_0, 'in'))    
        self.msg_connect((self.blocks_tagged_stream_to_pdu_0_0, 'pdus'), (self.cc_sdr_cc_decoder_0, 'in'))    
        self.msg_connect((self.cc_sdr_cc_decoder_0, 'out'), (self.blocks_socket_pdu_0_0, 'pdus'))    
        self.msg_connect((self.cc_sdr_cc_encoder_0, 'out'), (self.blocks_pdu_to_tagged_stream_0, 'pdus'))    
        self.connect((self.blocks_complex_to_real_0, 0), (self.blocks_moving_average_xx_0, 0))    
        self.connect((self.blocks_moving_average_xx_0, 0), (self.blocks_nlog10_ff_0, 0))    
        self.connect((self.blocks_moving_average_xx_0, 0), (self.blocks_nlog10_ff_0_0, 0))    
        self.connect((self.blocks_multiply_conjugate_cc_0, 0), (self.blocks_complex_to_real_0, 0))    
        self.connect((self.blocks_nlog10_ff_0, 0), (self.qtgui_number_sink_0, 0))    
        self.connect((self.blocks_nlog10_ff_0_0, 0), (self.qtgui_number_sink_0, 1))    
        self.connect((self.blocks_pdu_to_tagged_stream_0, 0), (self.blocks_tagged_stream_multiply_length_0, 0))    
        self.connect((self.blocks_tagged_stream_multiply_length_0, 0), (self.digital_gfsk_mod_1, 0))    
        self.connect((self.blocks_tagged_stream_multiply_length_0_0, 0), (self.blocks_tagged_stream_to_pdu_0_0, 0))    
        self.connect((self.blocks_unpacked_to_packed_xx_0_0, 0), (self.blocks_tagged_stream_multiply_length_0_0, 0))    
        self.connect((self.cc_sdr_fixedlen_packet_synchronizer_0, 0), (self.blocks_unpacked_to_packed_xx_0_0, 0))    
        self.connect((self.digital_correlate_access_code_tag_bb_0_0, 0), (self.cc_sdr_fixedlen_packet_synchronizer_0, 0))    
        self.connect((self.digital_fll_band_edge_cc_0, 0), (self.freq_xlating_fir_filter_xxx_1, 0))    
        self.connect((self.digital_gfsk_demod_0, 0), (self.digital_correlate_access_code_tag_bb_0_0, 0))    
        self.connect((self.digital_gfsk_mod_1, 0), (self.low_pass_filter_0, 0))    
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.qtgui_sink_x_0_0, 0))    
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.rational_resampler_xxx_0_0, 0))    
        self.connect((self.freq_xlating_fir_filter_xxx_1, 0), (self.blocks_multiply_conjugate_cc_0, 0))    
        self.connect((self.freq_xlating_fir_filter_xxx_1, 0), (self.blocks_multiply_conjugate_cc_0, 1))    
        self.connect((self.freq_xlating_fir_filter_xxx_1, 0), (self.digital_gfsk_demod_0, 0))    
        self.connect((self.freq_xlating_fir_filter_xxx_1, 0), (self.qtgui_sink_x_0, 0))    
        self.connect((self.low_pass_filter_0, 0), (self.rational_resampler_xxx_0, 0))    
        self.connect((self.low_pass_filter_0_0, 0), (self.uhd_usrp_sink_0, 0))    
        self.connect((self.rational_resampler_xxx_0, 0), (self.low_pass_filter_0_0, 0))    
        self.connect((self.rational_resampler_xxx_0_0, 0), (self.digital_fll_band_edge_cc_0, 0))    
        self.connect((self.uhd_usrp_source_0, 0), (self.freq_xlating_fir_filter_xxx_0, 0))    

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "cc_packet")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()


    def get_filter_width(self):
        return self.filter_width

    def set_filter_width(self, filter_width):
        self.filter_width = filter_width
        self.freq_xlating_fir_filter_xxx_0.set_taps((firdes.low_pass(1, self.source_rate, self.filter_width/2.0, self.filter_width/20.0)))

    def get_rate(self):
        return self.rate

    def set_rate(self, rate):
        self.rate = rate
        self.freq_xlating_fir_filter_xxx_1.set_taps((firdes.low_pass(1, self.freq_comp_rate, self.rate*0.625, self.rate/20.0)))
        self.low_pass_filter_0.set_taps(firdes.low_pass(1, self.samp_rate, self.rate * 1.0, 1e3, firdes.WIN_HAMMING, 6.76))

    def get_threshold(self):
        return self.threshold

    def set_threshold(self, threshold):
        self.threshold = threshold

    def get_sps(self):
        return self.sps

    def set_sps(self, sps):
        self.sps = sps

    def get_source_rate(self):
        return self.source_rate

    def set_source_rate(self, source_rate):
        self.source_rate = source_rate
        self.low_pass_filter_0_0.set_taps(firdes.low_pass(1, self.source_rate, self.samp_rate/2.0, self.samp_rate/20.0, firdes.WIN_HAMMING, 6.76))
        self.uhd_usrp_sink_0.set_samp_rate(self.source_rate)
        self.uhd_usrp_source_0.set_samp_rate(self.source_rate)
        self.qtgui_sink_x_0_0.set_frequency_range(0, self.source_rate)
        self.freq_xlating_fir_filter_xxx_0.set_taps((firdes.low_pass(1, self.source_rate, self.filter_width/2.0, self.filter_width/20.0)))

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.low_pass_filter_0.set_taps(firdes.low_pass(1, self.samp_rate, self.rate * 1.0, 1e3, firdes.WIN_HAMMING, 6.76))
        self.low_pass_filter_0_0.set_taps(firdes.low_pass(1, self.source_rate, self.samp_rate/2.0, self.samp_rate/20.0, firdes.WIN_HAMMING, 6.76))
        self.qtgui_sink_x_0.set_frequency_range(0, self.samp_rate)

    def get_preamble(self):
        return self.preamble

    def set_preamble(self, preamble):
        self.preamble = preamble

    def get_packet_length(self):
        return self.packet_length

    def set_packet_length(self, packet_length):
        self.packet_length = packet_length

    def get_offset_rx(self):
        return self.offset_rx

    def set_offset_rx(self, offset_rx):
        self.offset_rx = offset_rx
        self.freq_xlating_fir_filter_xxx_0.set_center_freq(50e3 - self.offset_rx)

    def get_freq_comp_rate(self):
        return self.freq_comp_rate

    def set_freq_comp_rate(self, freq_comp_rate):
        self.freq_comp_rate = freq_comp_rate
        self.freq_xlating_fir_filter_xxx_1.set_taps((firdes.low_pass(1, self.freq_comp_rate, self.rate*0.625, self.rate/20.0)))

    def get_fec(self):
        return self.fec

    def set_fec(self, fec):
        self.fec = fec

    def get_access_code(self):
        return self.access_code

    def set_access_code(self, access_code):
        self.access_code = access_code


def argument_parser():
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option(
        "", "--filter-width", dest="filter_width", type="eng_float", default=eng_notation.num_to_str(96e3),
        help="Set FM filter width [default=%default]")
    parser.add_option(
        "", "--rate", dest="rate", type="eng_float", default=eng_notation.num_to_str(9600),
        help="Set Data Rate [default=%default]")
    return parser


def main(top_block_cls=cc_packet, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls(filter_width=options.filter_width, rate=options.rate)
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()
