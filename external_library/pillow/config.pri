#
# Configurable Pillow Features
#

# Uncomment the following to build pillow as a static lib instead of a shared libary
#CONFIG += pillow_static

# Comment/uncomment the following line to enable SSL support in Pillow.
CONFIG += pillow_ssl

# Comment/uncomment the following lines to enable Zlib support (currently for transparent decompression of gzip streams in HttpClient).
#CONFIG += pillow_zlib
#PILLOW_ZLIB_LIBS = -lz

#
# Project Setup (not configurable)
#

#CONFIG -= debug_and_release

!pillow_ssl: CONFIG += pillow_no_ssl		# Previous CONFIG option was 'pillow_no_ssl'.
pillow_no_ssl: DEFINES += PILLOW_NO_SSL

pillow_zlib: DEFINES += PILLOW_ZLIB

PILLOWCORE_LIB_NAME = pillowcore
CONFIG(debug, debug|release) {
        TARGET = $${TARGET} # Append a "d" suffix on debug libs.
        PILLOWCORE_LIB_NAME = $${PILLOWCORE_LIB_NAME}
}

contains(QMAKE_CC, cl) {
	PILLOWCORE_LIB_FILE = $${PILLOWCORE_LIB_NAME}.lib
} else {
	pillow_static: PILLOWCORE_LIB_FILE = lib$${PILLOWCORE_LIB_NAME}.a
	!pillow_static: PILLOWCORE_LIB_FILE = lib$${PILLOWCORE_LIB_NAME}.so
	QMAKE_CXXFLAGS += --std=c++11
}
