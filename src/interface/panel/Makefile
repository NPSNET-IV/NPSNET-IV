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
			-I../../communication/include

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	basic_panel.cc
TARGETS =       $(LIBDIR)libbasic_panel$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	basic_panel$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

basic_panel$(GL_EXT)$(BIN_EXT).o: ../include/basic_panel.h /usr/include/Performer/pf.h
basic_panel$(GL_EXT)$(BIN_EXT).o: ../../communication/include/socket_struct.h
basic_panel$(GL_EXT)$(BIN_EXT).o: ../include/input_device.h ../include/nps_device.h
basic_panel$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h
basic_panel$(GL_EXT)$(BIN_EXT).o: ../../communication/include/socket_lib.h
