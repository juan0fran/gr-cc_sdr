#!/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/cubecat/Escritorio/GNURadio/gr-cc_sdr/python
export GR_CONF_CONTROLPORT_ON=False
export PATH=/home/cubecat/Escritorio/GNURadio/gr-cc_sdr/build/python:$PATH
export LD_LIBRARY_PATH=/home/cubecat/Escritorio/GNURadio/gr-cc_sdr/build/lib:$LD_LIBRARY_PATH
export PYTHONPATH=/home/cubecat/Escritorio/GNURadio/gr-cc_sdr/build/swig:$PYTHONPATH
/usr/bin/python2 /home/cubecat/Escritorio/GNURadio/gr-cc_sdr/python/qa_cc_decoder.py 
