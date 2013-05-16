# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE        =       Makefile

# You can change these flags while testing for optimizations
#OPT    =       -O2 -ansi
#OPT    =       -g
#CFLAGS =       $(OPT)

# You can locally define any symbols you need using the next line
#DEFINES        =

# You can define include directories your code needs here --
#    other than ../include
APPINCDIRS      =       -I/usr/include/Performer -I/usr/src/Performer/include \
                        -I../../apps/include -I../../communication/include \
                        -I../../environment/include -I../../interface/include \
                        -I../../entities/include -I../../setup/include \
                        -I../../display/include -I../../c2/include 

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES =       screen_dump.cc time_funcs.cc automateClass.cc npsIOStream.cc \
		transportClass.cc
TARGETS =       $(LIBDIR)libnps_genutil$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS =       screen_dump$(GL_EXT)$(BIN_EXT).o \
                time_funcs$(GL_EXT)$(BIN_EXT).o \
		automateClass$(GL_EXT)$(BIN_EXT).o \
		npsIOStream$(GL_EXT)$(BIN_EXT).o \
		transportClass$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post

# DO NOT DELETE

screen_dump.o: /usr/include/Performer/pfutil.h
screen_dump.o: ../../apps/include/common_defs.h /usr/include/Performer/pf.h
screen_dump.o: ../../communication/include/pdu.h
screen_dump.o: ../../communication/include/envirvalues.h
screen_dump.o: ../../communication/include/rwstruct.h
screen_dump.o: ../../setup/include/setup_const.h
screen_dump.o: ../../display/include/display_const.h
screen_dump.o: ../../interface/include/nps_device.h
screen_dump.o: ../../c2/include/highlight.h ../../c2/include/c2_const.h
screen_dump.o: ../../communication/include/identify.h
screen_dump.o: ../../display/include/XCursorUtil.h
screen_dump.o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
screen_dump.o: /usr/include/Performer/opengl.h
screen_dump.o: ../../interface/include/FastrakClass.h
screen_dump.o: ../../interface/include/fakespaceClass.h
screen_dump.o: ../../interface/include/input_device.h
screen_dump.o: ../../interface/include/lw_tcpip.h
screen_dump.o: ../../communication/include/ip_co_sockcomm.h
screen_dump.o: ../../communication/include/ip_sockcomm.h
screen_dump.o: ../../communication/include/sockcomm.h
screen_dump.o: ../../communication/include/co_sockcomm.h
screen_dump.o: ../../interface/include/lw-nps.h ../include/screen_dump.h
time_funcs.o: ../include/time_funcs.h ../include/conversion_const.h
automateClass.o: /usr/include/Performer/pf.h ../include/npsIOStream.h
automateClass.o: ../include/automateClass.h ../include/infoClass.h
automateClass.o: ../include/transportClass.h
npsIOStream.o: /usr/include/Performer/pf.h ../include/npsIOStream.h
transportClass.o: /usr/include/Performer/pf.h ../include/npsIOStream.h
transportClass.o: ../include/transportClass.h ../include/infoClass.h
