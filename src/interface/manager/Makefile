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
			-I../../communication/include \
			-I../../../otherlibs/ISMS \
			-I../../../otherlibs/ISMS/Mcast

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	input_manager.cc
TARGETS =       $(LIBDIR)libinput_manager$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	input_manager$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

input_manager$(GL_EXT)$(BIN_EXT).o: ../include/input_manager.h /usr/include/Performer/pf.h
input_manager$(GL_EXT)$(BIN_EXT).o: ../include/nps_device.h /usr/include/Performer/pfutil.h
input_manager$(GL_EXT)$(BIN_EXT).o: ../include/input_device.h ../include/keyboard.h
input_manager$(GL_EXT)$(BIN_EXT).o: ../include/fcs.h ../include/flybox.h ../include/lv3.h
input_manager$(GL_EXT)$(BIN_EXT).o: ../include/leather_flybox.h ../include/joystick.h
input_manager$(GL_EXT)$(BIN_EXT).o: ../include/kaflight.h ../include/iportClass.h
input_manager$(GL_EXT)$(BIN_EXT).o: ../include/DI_comm.h ../../communication/include/pdu.h
input_manager$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
input_manager$(GL_EXT)$(BIN_EXT).o: ../include/testportClass.h ../include/ptpClass.h
input_manager$(GL_EXT)$(BIN_EXT).o: ../include/uniportClass.h
input_manager$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/ISMS/Mcast/Mcast.h
input_manager$(GL_EXT)$(BIN_EXT).o: ../include/UpperBodyClass.h ../include/RifleClass.h
input_manager$(GL_EXT)$(BIN_EXT).o: ../include/ArmClass.h ../include/FastrakClass.h
input_manager$(GL_EXT)$(BIN_EXT).o: ../include/basic_panel.h
input_manager$(GL_EXT)$(BIN_EXT).o: ../../communication/include/socket_struct.h
input_manager$(GL_EXT)$(BIN_EXT).o: ../include/fakespaceClass.h ../include/odt.h
input_manager$(GL_EXT)$(BIN_EXT).o: ../include/lw_tcpip.h
input_manager$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
input_manager$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
input_manager$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
input_manager$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
input_manager$(GL_EXT)$(BIN_EXT).o: ../include/lw-nps.h
