#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2016 <+YOU OR YOUR COMPANY+>.
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

import numpy
from gnuradio import gr
import pmt

import collections

class fixedlen_packet_syncword_preamble(gr.basic_block):
    """
    docstring for block fixedlen_packet_syncword_preamble
    """
    def __init__(self, plentag, plen, preamble_bytes, access_code, streamtype):
        gr.basic_block.__init__(self,
            name="fixedlen_packet_syncword_preamble",
            in_sig=[streamtype],
            out_sig=[streamtype])
        self.packetlen_tag = pmt.string_to_symbol(plentag)
        self.access_code = access_code
        self.packet_len = plen
        self.preamble_bytes = preamble_bytes;
        self.stream = collections.deque(maxlen=plen - 1)
        self.maxtag = 0
        self.data = []
        self.tags = []
        self.written = 0
        self.preamble = []
        for i in range(preamble_bytes):
            self.preamble += "10101010"

    def general_work(self, input_items, output_items):
        inp = input_items[0]
        out = output_items[0]

        window = list(self.stream) + inp.tolist()

        alltags = self.get_tags_in_range(0, self.maxtag, self.nitems_read(0) + len(inp), self.packetlen_tag)
        for tag in alltags:
            if tag.offset not in self.tags:
                self.maxtag = max(self.maxtag, tag.offset)
                self.tags.append(tag.offset)
        for tag in self.tags:
            if (tag >= self.nitems_read(0) - len(self.stream)) and (tag < self.nitems_read(0) + len(inp) - self.packet_len + 1):
                self.tags.remove(tag)
                start = tag - self.nitems_read(0) + len(self.stream)
                packet = window[start : start + self.packet_len]
                self.data += list(self.preamble)
                self.data += list(self.access_code)
                self.data += packet
                self.written += self.packet_len
                self.add_item_tag(0, self.written, self.packetlen_tag, pmt.from_long(self.packet_len))

        self.stream.extend(inp.tolist())

        len_write = min(len(self.data), len(output_items[0]))
        output_items[0][:len_write] = self.data[:len_write]
        self.data = self.data[len_write:]
        self.consume(0, len(inp))
        return len_write
