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
APPINCDIRS	=	-I/usr/include/Performer -I/usr/src/Performer/include

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	fcs.cc
TARGETS =       $(LIBDIR)libfcs$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	fcs$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

fcs$(GL_EXT)$(BIN_EXT).o: ../include/fcs.h ../include/input_device.h /usr/include/Performer/pf.h
fcs$(GL_EXT)$(BIN_EXT).o: ../include/nps_device.h /usr/include/Performer/pfutil.h
