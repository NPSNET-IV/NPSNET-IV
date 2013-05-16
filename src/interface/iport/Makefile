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
			-I../../setup/include \
			-I../../entities/include \
			-I../../display/include \
			-I../../apps/include \
			-I../../../otherlibs/ISMS/Mcast \
			-I../../c2/include

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	iportClass.cc uniportClass.cc testportClass.cc ptpClass.cc
TARGETS =       $(LIBDIR)libiport$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	iportClass$(GL_EXT)$(BIN_EXT).o \
		uniportClass$(GL_EXT)$(BIN_EXT).o \
		testportClass$(GL_EXT)$(BIN_EXT).o \
		ptpClass$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

iportClass$(GL_EXT)$(BIN_EXT).o: ../include/iportClass.h ../include/input_device.h
iportClass$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h ../include/nps_device.h
iportClass$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../include/DI_comm.h
iportClass$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
iportClass$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
iportClass$(GL_EXT)$(BIN_EXT).o: ../../entities/include/jointmods.h
uniportClass$(GL_EXT)$(BIN_EXT).o: ../include/input_device.h /usr/include/Performer/pf.h
uniportClass$(GL_EXT)$(BIN_EXT).o: ../include/nps_device.h /usr/include/Performer/pfutil.h
uniportClass$(GL_EXT)$(BIN_EXT).o: ../include/iportClass.h ../include/DI_comm.h
uniportClass$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
uniportClass$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
uniportClass$(GL_EXT)$(BIN_EXT).o: ../include/uniportClass.h
uniportClass$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/ISMS/Mcast/Mcast.h
testportClass$(GL_EXT)$(BIN_EXT).o: ../include/iportClass.h ../include/input_device.h
testportClass$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h ../include/nps_device.h
testportClass$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../include/DI_comm.h
testportClass$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
testportClass$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
testportClass$(GL_EXT)$(BIN_EXT).o: ../include/testportClass.h ../include/ptpClass.h
ptpClass$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h ../include/ptpClass.h
ptpClass$(GL_EXT)$(BIN_EXT).o: ../../entities/include/jointmods.h ../../display/include/view.h
ptpClass$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
ptpClass$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
ptpClass$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
ptpClass$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
ptpClass$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
ptpClass$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
ptpClass$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h ../include/nps_device.h
ptpClass$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../c2/include/highlight.h
ptpClass$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h
ptpClass$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
ptpClass$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h ../include/hmdClass.h
ptpClass$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h /usr/include/Performer/opengl.h
ptpClass$(GL_EXT)$(BIN_EXT).o: ../include/FastrakClass.h ../include/fakespaceClass.h
ptpClass$(GL_EXT)$(BIN_EXT).o: ../include/input_device.h ../include/lw_tcpip.h
ptpClass$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
ptpClass$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
ptpClass$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
ptpClass$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h ../include/lw-nps.h
ptpClass$(GL_EXT)$(BIN_EXT).o: ../include/input_manager.h ../include/DI_comm.h
