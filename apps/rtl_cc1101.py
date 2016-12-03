#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: CC1101 decoder
# Author: Juan Fran Muñoz
# Description: CC1101 decoder
# Generated: Sat Dec  3 11:41:12 2016
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

import os
import sys
sys.path.append(os.environ.get('GRC_HIER_PATH', os.path.expanduser('~/.grc_gnuradio')))

from PyQt4 import Qt
from gnuradio import analog
from gnuradio import blocks
from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio import qtgui
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.qtgui import Range, RangeWidget
from optparse import OptionParser
from p_meter import p_meter  # grc-generated hier_block
import cc_sdr
import math
import numpy
import osmosdr
import sip
import sql_writer
import time


class rtl_cc1101(gr.top_block, Qt.QWidget):

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

        self.settings = Qt.QSettings("GNU Radio", "rtl_cc1101")
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
        self.var_om = var_om = 0.25
        self.threshold = threshold = 3
        self.source_rate = source_rate = 1e6
        self.samp_rate = samp_rate = rate*sps
        self.packet_length = packet_length = 255
        self.mu_var = mu_var = 0.175
        self.freq_shift = freq_shift = 0
        self.freq_comp_rate = freq_comp_rate = 96e3
        self.fec = fec = 1
        self.access_code = access_code = "11010011100100011101001110010001"

        ##################################################
        # Blocks
        ##################################################
        self._var_om_range = Range(0, 0.5, 0.001, 0.25, 200)
        self._var_om_win = RangeWidget(self._var_om_range, self.set_var_om, "OMEGA", "counter_slider", float)
        self.top_layout.addWidget(self._var_om_win)
        self._mu_var_range = Range(0, 0.25, 0.001, 0.175, 200)
        self._mu_var_win = RangeWidget(self._mu_var_range, self.set_mu_var, "MU", "counter_slider", float)
        self.top_layout.addWidget(self._mu_var_win)
        self._freq_shift_range = Range(-200e3, 200e3, 500, 0, 200)
        self._freq_shift_win = RangeWidget(self._freq_shift_range, self.set_freq_shift, "Frequency Shift", "counter_slider", float)
        self.top_layout.addWidget(self._freq_shift_win)
        self.sql_writer_sql_beacon_parser_0 = sql_writer.sql_beacon_parser("root", "root", "nslsbp", "hk_data", "localhost", 0)
        self.rtlsdr_source_0 = osmosdr.source( args="numchan=" + str(1) + " " + "" )
        self.rtlsdr_source_0.set_sample_rate(source_rate)
        self.rtlsdr_source_0.set_center_freq(freq-offset, 0)
        self.rtlsdr_source_0.set_freq_corr(0, 0)
        self.rtlsdr_source_0.set_dc_offset_mode(0, 0)
        self.rtlsdr_source_0.set_iq_balance_mode(0, 0)
        self.rtlsdr_source_0.set_gain_mode(False, 0)
        self.rtlsdr_source_0.set_gain(40, 0)
        self.rtlsdr_source_0.set_if_gain(20, 0)
        self.rtlsdr_source_0.set_bb_gain(20, 0)
        self.rtlsdr_source_0.set_antenna("", 0)
        self.rtlsdr_source_0.set_bandwidth(0, 0)
          
        self.rational_resampler_xxx_0 = filter.rational_resampler_ccc(
                interpolation=int(freq_comp_rate),
                decimation=int(source_rate),
                taps=None,
                fractional_bw=None,
        )
        self.qtgui_sink_x_0_0_0 = qtgui.sink_c(
        	1024, #fftsize
        	firdes.WIN_BLACKMAN_hARRIS, #wintype
        	0, #fc
        	samp_rate, #bw
        	"", #name
        	True, #plotfreq
        	True, #plotwaterfall
        	False, #plottime
        	False, #plotconst
        )
        self.qtgui_sink_x_0_0_0.set_update_time(1.0/10)
        self._qtgui_sink_x_0_0_0_win = sip.wrapinstance(self.qtgui_sink_x_0_0_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_sink_x_0_0_0_win)
        
        self.qtgui_sink_x_0_0_0.enable_rf_freq(False)
        
        
          
        self.qtgui_sink_x_0_0 = qtgui.sink_c(
        	1024, #fftsize
        	firdes.WIN_BLACKMAN_hARRIS, #wintype
        	0, #fc
        	source_rate, #bw
        	"", #name
        	True, #plotfreq
        	True, #plotwaterfall
        	False, #plottime
        	False, #plotconst
        )
        self.qtgui_sink_x_0_0.set_update_time(1.0/10)
        self._qtgui_sink_x_0_0_win = sip.wrapinstance(self.qtgui_sink_x_0_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_sink_x_0_0_win)
        
        self.qtgui_sink_x_0_0.enable_rf_freq(False)
        
        
          
        self.qtgui_number_sink_0_0 = qtgui.number_sink(
            gr.sizeof_float,
            0.95,
            qtgui.NUM_GRAPH_HORIZ,
            1
        )
        self.qtgui_number_sink_0_0.set_update_time(0.1)
        self.qtgui_number_sink_0_0.set_title("Floor Power")
        
        labels = ["Raw", "pDiff", "Raw", "", "",
                  "", "", "", "", ""]
        units = ["dB", "dB", "dB", "", "",
                 "", "", "", "", ""]
        colors = [("blue", "red"), ("black", "red"), ("black", "red"), ("black", "black"), ("black", "black"),
                  ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black")]
        factor = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        for i in xrange(1):
            self.qtgui_number_sink_0_0.set_min(i, -174+30)
            self.qtgui_number_sink_0_0.set_max(i, -20)
            self.qtgui_number_sink_0_0.set_color(i, colors[i][0], colors[i][1])
            if len(labels[i]) == 0:
                self.qtgui_number_sink_0_0.set_label(i, "Data {0}".format(i))
            else:
                self.qtgui_number_sink_0_0.set_label(i, labels[i])
            self.qtgui_number_sink_0_0.set_unit(i, units[i])
            self.qtgui_number_sink_0_0.set_factor(i, factor[i])
        
        self.qtgui_number_sink_0_0.enable_autoscale(False)
        self._qtgui_number_sink_0_0_win = sip.wrapinstance(self.qtgui_number_sink_0_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_number_sink_0_0_win)
        self.p_meter_0_0 = p_meter(
            samp_rate=samp_rate,
        )
        self.freq_xlating_fir_filter_xxx_1 = filter.freq_xlating_fir_filter_ccc(int(freq_comp_rate/samp_rate), (firdes.low_pass(1, freq_comp_rate, rate, rate/20.0)), 0, freq_comp_rate)
        self.freq_xlating_fir_filter_xxx_0_0 = filter.freq_xlating_fir_filter_ccc(1, (1, ), offset + freq_shift, source_rate)
        self.freq_xlating_fir_filter_xxx_0 = filter.freq_xlating_fir_filter_ccc(1, (firdes.low_pass(1, source_rate, filter_width/2.0, filter_width/20.0)), offset + freq_shift, source_rate)
        self.digital_fll_band_edge_cc_0 = digital.fll_band_edge_cc(freq_comp_rate / rate, 0.35, 25, 0.01)
        self.digital_correlate_access_code_tag_bb_0_0 = digital.correlate_access_code_tag_bb(access_code, threshold, "syncword")
        self.digital_clock_recovery_mm_xx_0 = digital.clock_recovery_mm_ff(sps*(1+0.0), var_om*mu_var*mu_var, 0.5, mu_var, 0.005)
        self.digital_binary_slicer_fb_0 = digital.binary_slicer_fb()
        self.cc_sdr_fixedlen_packet_synchronizer_0 = cc_sdr.fixedlen_packet_synchronizer("syncword", "packet_len", (packet_length + (1-fec)*2 +  fec*(packet_length + 8)) * 8, numpy.byte)
        self.cc_sdr_cc_decoder_0 = cc_sdr.cc_decoder(True, True, True, packet_length)
        self.blocks_unpacked_to_packed_xx_0_0 = blocks.unpacked_to_packed_bb(1, gr.GR_MSB_FIRST)
        self.blocks_tagged_stream_to_pdu_0_0 = blocks.tagged_stream_to_pdu(blocks.byte_t, "packet_len")
        self.blocks_tagged_stream_multiply_length_0_0 = blocks.tagged_stream_multiply_length(gr.sizeof_char*1, "packet_len", 1/8.0)
        self.analog_quadrature_demod_cf_0 = analog.quadrature_demod_cf(sps/(math.pi/2))

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.blocks_tagged_stream_to_pdu_0_0, 'pdus'), (self.cc_sdr_cc_decoder_0, 'in'))    
        self.msg_connect((self.cc_sdr_cc_decoder_0, 'out'), (self.sql_writer_sql_beacon_parser_0, 'in'))    
        self.connect((self.analog_quadrature_demod_cf_0, 0), (self.digital_clock_recovery_mm_xx_0, 0))    
        self.connect((self.blocks_tagged_stream_multiply_length_0_0, 0), (self.blocks_tagged_stream_to_pdu_0_0, 0))    
        self.connect((self.blocks_unpacked_to_packed_xx_0_0, 0), (self.blocks_tagged_stream_multiply_length_0_0, 0))    
        self.connect((self.cc_sdr_fixedlen_packet_synchronizer_0, 0), (self.blocks_unpacked_to_packed_xx_0_0, 0))    
        self.connect((self.digital_binary_slicer_fb_0, 0), (self.digital_correlate_access_code_tag_bb_0_0, 0))    
        self.connect((self.digital_clock_recovery_mm_xx_0, 0), (self.digital_binary_slicer_fb_0, 0))    
        self.connect((self.digital_correlate_access_code_tag_bb_0_0, 0), (self.cc_sdr_fixedlen_packet_synchronizer_0, 0))    
        self.connect((self.digital_fll_band_edge_cc_0, 0), (self.freq_xlating_fir_filter_xxx_1, 0))    
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.rational_resampler_xxx_0, 0))    
        self.connect((self.freq_xlating_fir_filter_xxx_0_0, 0), (self.qtgui_sink_x_0_0, 0))    
        self.connect((self.freq_xlating_fir_filter_xxx_1, 0), (self.analog_quadrature_demod_cf_0, 0))    
        self.connect((self.freq_xlating_fir_filter_xxx_1, 0), (self.p_meter_0_0, 0))    
        self.connect((self.freq_xlating_fir_filter_xxx_1, 0), (self.qtgui_sink_x_0_0_0, 0))    
        self.connect((self.p_meter_0_0, 0), (self.qtgui_number_sink_0_0, 0))    
        self.connect((self.rational_resampler_xxx_0, 0), (self.digital_fll_band_edge_cc_0, 0))    
        self.connect((self.rtlsdr_source_0, 0), (self.freq_xlating_fir_filter_xxx_0, 0))    
        self.connect((self.rtlsdr_source_0, 0), (self.freq_xlating_fir_filter_xxx_0_0, 0))    

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "rtl_cc1101")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()


    def get_filter_width(self):
        return self.filter_width

    def set_filter_width(self, filter_width):
        self.filter_width = filter_width
        self.freq_xlating_fir_filter_xxx_0.set_taps((firdes.low_pass(1, self.source_rate, self.filter_width/2.0, self.filter_width/20.0)))

    def get_freq(self):
        return self.freq

    def set_freq(self, freq):
        self.freq = freq
        self.rtlsdr_source_0.set_center_freq(self.freq-self.offset, 0)

    def get_ip(self):
        return self.ip

    def set_ip(self, ip):
        self.ip = ip

    def get_offset(self):
        return self.offset

    def set_offset(self, offset):
        self.offset = offset
        self.freq_xlating_fir_filter_xxx_0_0.set_center_freq(self.offset + self.freq_shift)
        self.rtlsdr_source_0.set_center_freq(self.freq-self.offset, 0)
        self.freq_xlating_fir_filter_xxx_0.set_center_freq(self.offset + self.freq_shift)

    def get_port(self):
        return self.port

    def set_port(self, port):
        self.port = port

    def get_rate(self):
        return self.rate

    def set_rate(self, rate):
        self.rate = rate
        self.set_samp_rate(self.rate*self.sps)
        self.freq_xlating_fir_filter_xxx_1.set_taps((firdes.low_pass(1, self.freq_comp_rate, self.rate, self.rate/20.0)))

    def get_rf_gain(self):
        return self.rf_gain

    def set_rf_gain(self, rf_gain):
        self.rf_gain = rf_gain

    def get_sps(self):
        return self.sps

    def set_sps(self, sps):
        self.sps = sps
        self.set_samp_rate(self.rate*self.sps)
        self.analog_quadrature_demod_cf_0.set_gain(self.sps/(math.pi/2))
        self.digital_clock_recovery_mm_xx_0.set_omega(self.sps*(1+0.0))

    def get_var_om(self):
        return self.var_om

    def set_var_om(self, var_om):
        self.var_om = var_om
        self.digital_clock_recovery_mm_xx_0.set_gain_omega(self.var_om*self.mu_var*self.mu_var)

    def get_threshold(self):
        return self.threshold

    def set_threshold(self, threshold):
        self.threshold = threshold

    def get_source_rate(self):
        return self.source_rate

    def set_source_rate(self, source_rate):
        self.source_rate = source_rate
        self.qtgui_sink_x_0_0.set_frequency_range(0, self.source_rate)
        self.rtlsdr_source_0.set_sample_rate(self.source_rate)
        self.freq_xlating_fir_filter_xxx_0.set_taps((firdes.low_pass(1, self.source_rate, self.filter_width/2.0, self.filter_width/20.0)))

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.p_meter_0_0.set_samp_rate(self.samp_rate)
        self.qtgui_sink_x_0_0_0.set_frequency_range(0, self.samp_rate)

    def get_packet_length(self):
        return self.packet_length

    def set_packet_length(self, packet_length):
        self.packet_length = packet_length

    def get_mu_var(self):
        return self.mu_var

    def set_mu_var(self, mu_var):
        self.mu_var = mu_var
        self.digital_clock_recovery_mm_xx_0.set_gain_omega(self.var_om*self.mu_var*self.mu_var)
        self.digital_clock_recovery_mm_xx_0.set_gain_mu(self.mu_var)

    def get_freq_shift(self):
        return self.freq_shift

    def set_freq_shift(self, freq_shift):
        self.freq_shift = freq_shift
        self.freq_xlating_fir_filter_xxx_0_0.set_center_freq(self.offset + self.freq_shift)
        self.freq_xlating_fir_filter_xxx_0.set_center_freq(self.offset + self.freq_shift)

    def get_freq_comp_rate(self):
        return self.freq_comp_rate

    def set_freq_comp_rate(self, freq_comp_rate):
        self.freq_comp_rate = freq_comp_rate
        self.freq_xlating_fir_filter_xxx_1.set_taps((firdes.low_pass(1, self.freq_comp_rate, self.rate, self.rate/20.0)))

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


def main(top_block_cls=rtl_cc1101, options=None):
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
