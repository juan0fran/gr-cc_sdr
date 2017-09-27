#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Top Block
# Generated: Wed Sep 27 12:39:36 2017
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
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import cc_sdr
import numpy
import sys


class top_block(gr.top_block, Qt.QWidget):

    def __init__(self, param_threshold=3, param_packet_len=223, access_code='11010011100100011101001110010001'):
        gr.top_block.__init__(self, "Top Block")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Top Block")
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

        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())

        ##################################################
        # Parameters
        ##################################################
        self.param_threshold = param_threshold
        self.param_packet_len = param_packet_len
        self.access_code = access_code

        ##################################################
        # Blocks
        ##################################################
        self.digital_correlate_access_code_tag_bb_0_0 = digital.correlate_access_code_tag_bb(access_code, param_threshold, "syncword")
        self.cc_sdr_fixedlen_packet_synchronizer_0 = cc_sdr.fixedlen_packet_synchronizer("syncword", "packet_len", (param_packet_len+32)*8, numpy.byte)
        self.cc_sdr_cc_decoder_0 = cc_sdr.cc_decoder(False, True, True, param_packet_len)
        self.blocks_unpacked_to_packed_xx_0_0 = blocks.unpacked_to_packed_bb(1, gr.GR_MSB_FIRST)
        self.blocks_tagged_stream_to_pdu_0_0 = blocks.tagged_stream_to_pdu(blocks.byte_t, "packet_len")
        self.blocks_tagged_stream_multiply_length_0_0 = blocks.tagged_stream_multiply_length(gr.sizeof_char*1, "packet_len", 1/8.0)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.blocks_tagged_stream_to_pdu_0_0, 'pdus'), (self.cc_sdr_cc_decoder_0, 'in'))    
        self.msg_connect((self.cc_sdr_cc_decoder_0, 'out'), (self, 'out'))    
        self.connect((self.blocks_tagged_stream_multiply_length_0_0, 0), (self.blocks_tagged_stream_to_pdu_0_0, 0))    
        self.connect((self.blocks_unpacked_to_packed_xx_0_0, 0), (self.blocks_tagged_stream_multiply_length_0_0, 0))    
        self.connect((self.cc_sdr_fixedlen_packet_synchronizer_0, 0), (self.blocks_unpacked_to_packed_xx_0_0, 0))    
        self.connect((self.digital_correlate_access_code_tag_bb_0_0, 0), (self.cc_sdr_fixedlen_packet_synchronizer_0, 0))    
        self.connect((self, 0), (self.digital_correlate_access_code_tag_bb_0_0, 0))    

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()


    def get_param_threshold(self):
        return self.param_threshold

    def set_param_threshold(self, param_threshold):
        self.param_threshold = param_threshold

    def get_param_packet_len(self):
        return self.param_packet_len

    def set_param_packet_len(self, param_packet_len):
        self.param_packet_len = param_packet_len

    def get_access_code(self):
        return self.access_code

    def set_access_code(self, access_code):
        self.access_code = access_code


def argument_parser():
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option(
        "", "--param-threshold", dest="param_threshold", type="intx", default=3,
        help="Set threshold [default=%default]")
    parser.add_option(
        "", "--param-packet-len", dest="param_packet_len", type="intx", default=223,
        help="Set pack_len [default=%default]")
    parser.add_option(
        "", "--access-code", dest="access_code", type="string", default='11010011100100011101001110010001',
        help="Set 11010011100100011101001110010001 [default=%default]")
    return parser


def main(top_block_cls=top_block, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls(param_threshold=options.param_threshold, param_packet_len=options.param_packet_len, access_code=options.access_code)
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()
