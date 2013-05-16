# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
#OPT	=	-O2 -w
#OPT	=	-g
#CFLAGS	=	$(OPT)

# You can locally define any symbols you need using the next line
#DEFINES	=	-DDEBUG -DSHOW_DATA -DTRACE -DSTATS -DVTRACE
DEFINES	=	-DVTRACE

# You can define include directories your code needs here --
#    other than ../include
APPINCDIRS	=	-I/usr/include/Performer -I/usr/src/Performer/include \
			-I../../communication/include

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	lw_tcpip.cc
TARGETS =       $(LIBDIR)liblw_tcpip$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	lw_tcpip$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

lw_tcpip$(BIN_EXT).o: ../include/lw_tcpip.h
lw_tcpip$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
lw_tcpip$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
lw_tcpip$(BIN_EXT).o: ../../communication/include/sockcomm.h
lw_tcpip$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
lw_tcpip$(BIN_EXT).o: ../include/input_device.h /usr/include/Performer/pf.h
lw_tcpip$(BIN_EXT).o: ../include/nps_device.h /usr/include/Performer/pfutil.h
lw_tcpip$(BIN_EXT).o: ../include/lw-nps.h ../../communication/include/alarmclock.h
