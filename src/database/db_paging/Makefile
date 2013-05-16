# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
#OPT	=	-O2
#CFLAGS	=	$(OPT)

# You can locally define any symbols you need using the next line
#DEFINES	=

# You can define include directories your code needs here --
#    other than ../include
APPINCDIRS	= 	-I../../apps/include -I../../communication/include \
		-I../../setup/include -I../../display/include \
		-I../../interface/include -I../../c2/include \
		-I../../entities/include/ -I/usr/include/Performer

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	dbPagingClass.cc dbStorageClass.cc dbUtil.cc dbPagingCallback.cc dbDeletePageClass.cc
TARGETS =       $(LIBDIR)libdbpaging$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	dbPagingClass$(GL_EXT)$(BIN_EXT).o \
		dbStorageClass$(GL_EXT)$(BIN_EXT).o \
		dbUtil$(GL_EXT)$(BIN_EXT).o \
		dbPagingCallback$(GL_EXT)$(BIN_EXT).o \
		dbDeletePageClass$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

dbPagingClass$(GL_EXT)$(BIN_EXT).o: ../include/dbUtil.h ../include/dbDeletePageClass.h
dbPagingClass$(GL_EXT)$(BIN_EXT).o: ../include/dbStorageClass.h ../include/tsg.h
dbPagingClass$(GL_EXT)$(BIN_EXT).o: ../include/tsgpf.h ../include/tsgBuf.h
dbPagingClass$(GL_EXT)$(BIN_EXT).o: ../include/dbPagingClass.h
dbStorageClass$(GL_EXT)$(BIN_EXT).o: ../include/dbUtil.h ../include/dbStorageClass.h
dbStorageClass$(GL_EXT)$(BIN_EXT).o: ../include/dbDeletePageClass.h ../include/tsg.h
dbStorageClass$(GL_EXT)$(BIN_EXT).o: ../include/tsgpf.h ../include/tsgBuf.h
dbUtil$(GL_EXT)$(BIN_EXT).o: ../include/dbUtil.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../../c2/include/highlight.h ../../c2/include/c2_const.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../include/dbPagingCallback.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../include/dbDeletePageClass.h ../include/tsg.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../include/tsgpf.h ../include/tsgBuf.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../include/dbPagingClass.h ../include/dbUtil.h
dbPagingCallback$(GL_EXT)$(BIN_EXT).o: ../include/dbStorageClass.h
dbDeletePageClass$(GL_EXT)$(BIN_EXT).o: ../include/dbUtil.h ../include/dbDeletePageClass.h
