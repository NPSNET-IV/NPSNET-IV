# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
#OPT	=	-O2 -w
#OPT	=	-g
#CFLAGS	=	$(OPT)

# You can locally define any symbols you need using the next line
#DEFINES	=	-DDEBUG -DSHOW_DATA -DTRACE -DSTATS -DVTRACE

# You can define include directories your code needs here --
#    other than ../include
APPINCDIRS	=	-I/usr/include/Performer -I/usr/src/Performer/include \
			-I../../apps/include -I../../c2/include \
        -I../../communication/include -I../../database/include \
        -I../../display/include -I../../entities/include \
        -I../../environment/include -I../../interface/include \
        -I../../media/include -I../../setup/include -I../../utilities/include \
        -I$(JACKML_INCDIR) -I$(IPORT_INCDIR) -I$(MCAST_INCDIR) \
        -I/usr/include/Performer -I/usr/src/Performer/include

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	keyboard.cc Xinputs.cc
TARGETS =       $(LIBDIR)libkeyboard$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	keyboard$(GL_EXT)$(BIN_EXT).o \
		Xinputs$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

keyboard$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h /usr/include/Performer/pf.h
keyboard$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
keyboard$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
keyboard$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
keyboard$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
keyboard$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h ../include/nps_device.h
keyboard$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../c2/include/highlight.h
keyboard$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h
keyboard$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
keyboard$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h ../include/hmdClass.h
keyboard$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h /usr/include/Performer/opengl.h
keyboard$(GL_EXT)$(BIN_EXT).o: ../include/FastrakClass.h ../include/fakespaceClass.h
keyboard$(GL_EXT)$(BIN_EXT).o: ../include/input_device.h ../include/lw_tcpip.h
keyboard$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
keyboard$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
keyboard$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
keyboard$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h ../include/lw-nps.h
keyboard$(GL_EXT)$(BIN_EXT).o: ../include/keyboard.h
Xinputs$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h /usr/include/Performer/pf.h
Xinputs$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
Xinputs$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
Xinputs$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
Xinputs$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
Xinputs$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h ../include/nps_device.h
Xinputs$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../c2/include/highlight.h
Xinputs$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h ../../communication/include/identify.h
Xinputs$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h ../include/hmdClass.h
Xinputs$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h /usr/include/Performer/opengl.h
Xinputs$(GL_EXT)$(BIN_EXT).o: ../include/FastrakClass.h ../include/fakespaceClass.h
Xinputs$(GL_EXT)$(BIN_EXT).o: ../include/input_device.h ../include/lw_tcpip.h
Xinputs$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
Xinputs$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
Xinputs$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
Xinputs$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h ../include/lw-nps.h
Xinputs$(GL_EXT)$(BIN_EXT).o: ../include/keyboard.h
