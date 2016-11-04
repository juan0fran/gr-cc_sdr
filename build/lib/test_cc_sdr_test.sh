#!/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/cubecat/Escritorio/GNURadio/gr-cc_sdr/lib
export GR_CONF_CONTROLPORT_ON=False
export PATH=/home/cubecat/Escritorio/GNURadio/gr-cc_sdr/build/lib:$PATH
export LD_LIBRARY_PATH=/home/cubecat/Escritorio/GNURadio/gr-cc_sdr/build/lib:$LD_LIBRARY_PATH
export PYTHONPATH=$PYTHONPATH
test-cc_sdr 
