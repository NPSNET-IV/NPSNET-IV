# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
#OPT	=	-O2 -w
#OPT	=	-g
#CFLAGS	=	$(OPT)

# You can locally define any symbols you need using the next line
#DEFINES	=	-DDEBUG -DSHOW_DATA -DTRACE -DSTATS -DVTRACE
#DEFINES	=	-DSHOW_DATA

# You can define include directories your code needs here --
#    other than ../include
APPINCDIRS	=	-I/usr/include/Performer -I/usr/src/Performer/include

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	odt.cc
TARGETS =       $(LIBDIR)libodt$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	odt$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

odt$(GL_EXT)$(BIN_EXT).o: ../include/odt.h ../include/input_device.h /usr/include/Performer/pf.h
odt$(GL_EXT)$(BIN_EXT).o: ../include/nps_device.h /usr/include/Performer/pfutil.h
