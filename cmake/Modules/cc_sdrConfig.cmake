INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_CC_SDR cc_sdr)

FIND_PATH(
    CC_SDR_INCLUDE_DIRS
    NAMES cc_sdr/api.h
    HINTS $ENV{CC_SDR_DIR}/include
        ${PC_CC_SDR_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    CC_SDR_LIBRARIES
    NAMES gnuradio-cc_sdr
    HINTS $ENV{CC_SDR_DIR}/lib
        ${PC_CC_SDR_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CC_SDR DEFAULT_MSG CC_SDR_LIBRARIES CC_SDR_INCLUDE_DIRS)
MARK_AS_ADVANCED(CC_SDR_LIBRARIES CC_SDR_INCLUDE_DIRS)

