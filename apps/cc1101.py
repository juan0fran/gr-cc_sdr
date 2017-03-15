#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: CC1101 decoder
# Author: Juan Fran Muñoz
# Description: CC1101 decoder
# Generated: Tue Mar 14 15:41:43 2017
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


class cc1101(gr.top_block, Qt.QWidget):

    def __init__(self, filter_width=96e3, freq=430e6, ip="localhost", offset=50e3, port=7355, rate=9600, rf_gain=65):
        gr.top_block.__init__(self, "CC1101 decoder")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("CC1101 decoder")
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

        self.settings = Qt.QSettings("GNU Radio", "cc1101")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())

        ##################################################
        # Parameters
        ##################################################
        self.filter_width = filter_width
        self.freq = freq
        self.ip = ip
        self.offset = offset
        self.port = port
        self.rate = rate
        self.rf_gain = rf_gain

        ##################################################
        # Variables
        ##################################################
        self.sps = sps = 5
        self.source_rate = source_rate = 480e3
        self.threshold = threshold = 3
        self.samp_rate = samp_rate = rate*sps
        self.packet_length = packet_length = 255
        self.freq_shift = freq_shift = 0
        self.freq_comp_rate = freq_comp_rate = source_rate/5
        self.fec = fec = 0
        self.access_code = access_code = "11010011100100011101001110010001"

        ##################################################
        # Blocks
        ##################################################
        self.uhd_usrp_sink_0 = uhd.usrp_sink(
        	",".join(("", "")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(1),
        	),
        )
        self.uhd_usrp_sink_0.set_samp_rate(source_rate)
        self.uhd_usrp_sink_0.set_center_freq(freq, 0)
        self.uhd_usrp_sink_0.set_gain(rf_gain, 0)
        self.uhd_usrp_sink_0.set_antenna("TX/RX", 0)
        self.rational_resampler_xxx_0 = filter.rational_resampler_ccc(
                interpolation=10,
                decimation=1,
                taps=None,
                fractional_bw=None,
        )
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
        
        
          
        self._freq_shift_range = Range(-200e3, 200e3, 500, 0, 200)
        self._freq_shift_win = RangeWidget(self._freq_shift_range, self.set_freq_shift, "Frequency Shift", "counter_slider", float)
        self.top_layout.addWidget(self._freq_shift_win)
        self.digital_gfsk_mod_0 = digital.gfsk_mod(
        	samples_per_symbol=5,
        	sensitivity=0.5,
        	bt=0.5,
        	verbose=False,
        	log=False,
        )
        self.cc_sdr_fixedlen_packet_syncword_preamble_0 = cc_sdr.fixedlen_packet_syncword_preamble("packet_len2", 8*8 + (packet_length +  fec*(packet_length + 8)) * 8, 8, "11010011100100011101001110010001", numpy.byte)
        self.cc_sdr_cc_encoder_0 = cc_sdr.cc_encoder(False, True, True, packet_length)
        self.blocks_unpacked_to_packed_xx_0 = blocks.unpacked_to_packed_bb(1, gr.GR_MSB_FIRST)
        self.blocks_tagged_stream_multiply_length_0 = blocks.tagged_stream_multiply_length(gr.sizeof_char*1, "packet_len2", 8.0)
        self.blocks_socket_pdu_0 = blocks.socket_pdu("TCP_SERVER", "", "52001", 10000, False)
        self.blocks_pdu_to_tagged_stream_0 = blocks.pdu_to_tagged_stream(blocks.byte_t, "packet_len2")
        self.blocks_packed_to_unpacked_xx_0 = blocks.packed_to_unpacked_bb(1, gr.GR_MSB_FIRST)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.blocks_socket_pdu_0, 'pdus'), (self.cc_sdr_cc_encoder_0, 'in'))    
        self.msg_connect((self.cc_sdr_cc_encoder_0, 'out'), (self.blocks_pdu_to_tagged_stream_0, 'pdus'))    
        self.connect((self.blocks_packed_to_unpacked_xx_0, 0), (self.cc_sdr_fixedlen_packet_syncword_preamble_0, 0))    
        self.connect((self.blocks_pdu_to_tagged_stream_0, 0), (self.blocks_tagged_stream_multiply_length_0, 0))    
        self.connect((self.blocks_tagged_stream_multiply_length_0, 0), (self.blocks_packed_to_unpacked_xx_0, 0))    
        self.connect((self.blocks_unpacked_to_packed_xx_0, 0), (self.digital_gfsk_mod_0, 0))    
        self.connect((self.cc_sdr_fixedlen_packet_syncword_preamble_0, 0), (self.blocks_unpacked_to_packed_xx_0, 0))    
        self.connect((self.digital_gfsk_mod_0, 0), (self.qtgui_sink_x_0, 0))    
        self.connect((self.digital_gfsk_mod_0, 0), (self.rational_resampler_xxx_0, 0))    
        self.connect((self.rational_resampler_xxx_0, 0), (self.uhd_usrp_sink_0, 0))    

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "cc1101")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()


    def get_filter_width(self):
        return self.filter_width

    def set_filter_width(self, filter_width):
        self.filter_width = filter_width

    def get_freq(self):
        return self.freq

    def set_freq(self, freq):
        self.freq = freq
        self.uhd_usrp_sink_0.set_center_freq(self.freq, 0)

    def get_ip(self):
        return self.ip

    def set_ip(self, ip):
        self.ip = ip

    def get_offset(self):
        return self.offset

    def set_offset(self, offset):
        self.offset = offset

    def get_port(self):
        return self.port

    def set_port(self, port):
        self.port = port

    def get_rate(self):
        return self.rate

    def set_rate(self, rate):
        self.rate = rate
        self.set_samp_rate(self.rate*self.sps)

    def get_rf_gain(self):
        return self.rf_gain

    def set_rf_gain(self, rf_gain):
        self.rf_gain = rf_gain
        self.uhd_usrp_sink_0.set_gain(self.rf_gain, 0)
        	

    def get_sps(self):
        return self.sps

    def set_sps(self, sps):
        self.sps = sps
        self.set_samp_rate(self.rate*self.sps)

    def get_source_rate(self):
        return self.source_rate

    def set_source_rate(self, source_rate):
        self.source_rate = source_rate
        self.set_freq_comp_rate(self.source_rate/5)
        self.uhd_usrp_sink_0.set_samp_rate(self.source_rate)

    def get_threshold(self):
        return self.threshold

    def set_threshold(self, threshold):
        self.threshold = threshold

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.qtgui_sink_x_0.set_frequency_range(0, self.samp_rate)

    def get_packet_length(self):
        return self.packet_length

    def set_packet_length(self, packet_length):
        self.packet_length = packet_length

    def get_freq_shift(self):
        return self.freq_shift

    def set_freq_shift(self, freq_shift):
        self.freq_shift = freq_shift

    def get_freq_comp_rate(self):
        return self.freq_comp_rate

    def set_freq_comp_rate(self, freq_comp_rate):
        self.freq_comp_rate = freq_comp_rate

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
        "-f", "--freq", dest="freq", type="eng_float", default=eng_notation.num_to_str(430e6),
        help="Set frequency [default=%default]")
    parser.add_option(
        "", "--ip", dest="ip", type="string", default="localhost",
        help="Set UDP listen IP [default=%default]")
    parser.add_option(
        "", "--offset", dest="offset", type="eng_float", default=eng_notation.num_to_str(50e3),
        help="Set centre frequency offset [default=%default]")
    parser.add_option(
        "", "--port", dest="port", type="intx", default=7355,
        help="Set UDP port [default=%default]")
    parser.add_option(
        "", "--rate", dest="rate", type="eng_float", default=eng_notation.num_to_str(9600),
        help="Set Data Rate [default=%default]")
    parser.add_option(
        "", "--rf-gain", dest="rf_gain", type="eng_float", default=eng_notation.num_to_str(65),
        help="Set RF gain [default=%default]")
    return parser


def main(top_block_cls=cc1101, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls(filter_width=options.filter_width, freq=options.freq, ip=options.ip, offset=options.offset, port=options.port, rate=options.rate, rf_gain=options.rf_gain)
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()
