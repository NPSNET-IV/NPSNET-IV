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
			-I../../../otherlibs/Fakespace \
			-I../../c2/include

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	fakespaceClass.cc
TARGETS =       $(LIBDIR)libfakespace$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	fakespaceClass$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

fakespaceClass$(GL_EXT)$(BIN_EXT).o: ../include/fakespaceClass.h ../include/input_device.h
fakespaceClass$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h ../include/nps_device.h
fakespaceClass$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h
fakespaceClass$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/Fakespace/boom.h
