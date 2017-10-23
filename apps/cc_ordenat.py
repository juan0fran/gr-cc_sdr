#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Cc Ordenat
# Generated: Mon Oct 23 11:20:04 2017
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

from CC_9600_RX import CC_9600_RX  # grc-generated hier_block
from CC_9600_TX import CC_9600_TX  # grc-generated hier_block
from CC_Byte_Synchronizer import CC_Byte_Synchronizer  # grc-generated hier_block
from PyQt4 import Qt
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import qtgui
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import sip
import time
import usrp_control


class cc_ordenat(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Cc Ordenat")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Cc Ordenat")
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

        self.settings = Qt.QSettings("GNU Radio", "cc_ordenat")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 480e3
        self.lo_offset = lo_offset = 1e6
        self.freq_tx = freq_tx = 437.35e6
        self.freq_rx = freq_rx = 437.35e6

        ##################################################
        # Message Queues
        ##################################################
        uhd_amsg_source_0_msgq_out = blocks_message_source_0_msgq_in = gr.msg_queue(2)

        ##################################################
        # Blocks
        ##################################################
        self.usrp_control_pdu_to_pmt_0_0 = usrp_control.pdu_to_pmt()
        self.usrp_control_pdu_to_pmt_0 = usrp_control.pdu_to_pmt()
        self.uhd_usrp_source_0 = uhd.usrp_source(
        	",".join(("", "")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(1),
        	),
        )
        self.uhd_usrp_source_0.set_samp_rate(samp_rate)
        self.uhd_usrp_source_0.set_center_freq(freq_rx, 0)
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
        self.uhd_usrp_sink_0.set_samp_rate(samp_rate)
        self.uhd_usrp_sink_0.set_center_freq(freq_tx, 0)
        self.uhd_usrp_sink_0.set_gain(77.5, 0)
        self.uhd_usrp_sink_0.set_antenna("TX/RX", 0)
        self.uhd_amsg_source_0 = uhd.amsg_source(device_addr="", msgq=uhd_amsg_source_0_msgq_out)
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
        
        
          
        self.qtgui_number_sink_1 = qtgui.number_sink(
            gr.sizeof_float,
            0,
            qtgui.NUM_GRAPH_HORIZ,
            1
        )
        self.qtgui_number_sink_1.set_update_time(0.10)
        self.qtgui_number_sink_1.set_title("")
        
        labels = ["Frequency Shift", "", "", "", "",
                  "", "", "", "", ""]
        units = ["Hz", "", "", "", "",
                 "", "", "", "", ""]
        colors = [("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"),
                  ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black")]
        factor = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        for i in xrange(1):
            self.qtgui_number_sink_1.set_min(i, -5000)
            self.qtgui_number_sink_1.set_max(i, 5000)
            self.qtgui_number_sink_1.set_color(i, colors[i][0], colors[i][1])
            if len(labels[i]) == 0:
                self.qtgui_number_sink_1.set_label(i, "Data {0}".format(i))
            else:
                self.qtgui_number_sink_1.set_label(i, labels[i])
            self.qtgui_number_sink_1.set_unit(i, units[i])
            self.qtgui_number_sink_1.set_factor(i, factor[i])
        
        self.qtgui_number_sink_1.enable_autoscale(False)
        self._qtgui_number_sink_1_win = sip.wrapinstance(self.qtgui_number_sink_1.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_number_sink_1_win)
        self.qtgui_number_sink_0 = qtgui.number_sink(
            gr.sizeof_float,
            0,
            qtgui.NUM_GRAPH_HORIZ,
            2
        )
        self.qtgui_number_sink_0.set_update_time(0.10)
        self.qtgui_number_sink_0.set_title("")
        
        labels = ["Floor Power", "Band Power", "Noise Power", "", "",
                  "", "", "", "", ""]
        units = ["dBm", "dBm", "dBm", "", "",
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
        self.blocks_tagged_stream_to_pdu_0 = blocks.tagged_stream_to_pdu(blocks.byte_t, "packet_len")
        self.blocks_stream_to_tagged_stream_0 = blocks.stream_to_tagged_stream(gr.sizeof_char, 1, 56, "packet_len")
        self.blocks_socket_pdu_0_0_1 = blocks.socket_pdu("TCP_SERVER", "", "52000", 223, False)
        self.blocks_socket_pdu_0_0_0_0 = blocks.socket_pdu("UDP_SERVER", "", "52003", 255, False)
        self.blocks_socket_pdu_0_0_0 = blocks.socket_pdu("UDP_SERVER", "", "52002", 255, False)
        self.blocks_socket_pdu_0_0 = blocks.socket_pdu("UDP_SERVER", "", "52001", 223, False)
        self.blocks_socket_pdu_0 = blocks.socket_pdu("TCP_SERVER", "", "52004", 255, True)
        self.blocks_message_source_0 = blocks.message_source(gr.sizeof_char*1, blocks_message_source_0_msgq_in)
        self.CC_Byte_Synchronizer_0 = CC_Byte_Synchronizer(
            access_code='11010011100100011101001110010001',
            param_packet_len=223,
            param_threshold=3,
        )
        self.CC_9600_TX_0 = CC_9600_TX()
        self.CC_9600_RX_0 = CC_9600_RX(
            man_offset=0,
        )

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.CC_Byte_Synchronizer_0, 'out'), (self.blocks_socket_pdu_0_0_1, 'pdus'))    
        self.msg_connect((self.blocks_socket_pdu_0_0, 'pdus'), (self.CC_9600_TX_0, 'packet'))    
        self.msg_connect((self.blocks_socket_pdu_0_0_0, 'pdus'), (self.usrp_control_pdu_to_pmt_0, 'in'))    
        self.msg_connect((self.blocks_socket_pdu_0_0_0_0, 'pdus'), (self.usrp_control_pdu_to_pmt_0_0, 'in'))    
        self.msg_connect((self.blocks_tagged_stream_to_pdu_0, 'pdus'), (self.blocks_socket_pdu_0, 'pdus'))    
        self.msg_connect((self.usrp_control_pdu_to_pmt_0, 'out'), (self.uhd_usrp_source_0, 'command'))    
        self.msg_connect((self.usrp_control_pdu_to_pmt_0_0, 'out'), (self.uhd_usrp_sink_0, 'command'))    
        self.connect((self.CC_9600_RX_0, 0), (self.CC_Byte_Synchronizer_0, 0))    
        self.connect((self.CC_9600_RX_0, 1), (self.qtgui_number_sink_0, 1))    
        self.connect((self.CC_9600_RX_0, 2), (self.qtgui_number_sink_0, 0))    
        self.connect((self.CC_9600_RX_0, 3), (self.qtgui_number_sink_1, 0))    
        self.connect((self.CC_9600_TX_0, 0), (self.uhd_usrp_sink_0, 0))    
        self.connect((self.blocks_message_source_0, 0), (self.blocks_stream_to_tagged_stream_0, 0))    
        self.connect((self.blocks_stream_to_tagged_stream_0, 0), (self.blocks_tagged_stream_to_pdu_0, 0))    
        self.connect((self.uhd_usrp_source_0, 0), (self.CC_9600_RX_0, 0))    
        self.connect((self.uhd_usrp_source_0, 0), (self.qtgui_sink_x_0, 0))    

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "cc_ordenat")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()


    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.qtgui_sink_x_0.set_frequency_range(0, self.samp_rate)
        self.uhd_usrp_source_0.set_samp_rate(self.samp_rate)
        self.uhd_usrp_sink_0.set_samp_rate(self.samp_rate)

    def get_lo_offset(self):
        return self.lo_offset

    def set_lo_offset(self, lo_offset):
        self.lo_offset = lo_offset

    def get_freq_tx(self):
        return self.freq_tx

    def set_freq_tx(self, freq_tx):
        self.freq_tx = freq_tx
        self.uhd_usrp_sink_0.set_center_freq(self.freq_tx, 0)

    def get_freq_rx(self):
        return self.freq_rx

    def set_freq_rx(self, freq_rx):
        self.freq_rx = freq_rx
        self.uhd_usrp_source_0.set_center_freq(self.freq_rx, 0)


def main(top_block_cls=cc_ordenat, options=None):

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()
