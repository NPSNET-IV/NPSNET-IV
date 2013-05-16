# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
#OPT	=	-O2 -w
#OPT	=	-g
OPT	=
#CFLAGS	=	$(OPT)

# You can locally define any symbols you need using the next line
#DEFINES	=

# You can define include directories your code needs here --
#    other than ../include
APPINCDIRS	=	-I/usr/include/Performer -I/usr/src/Performer/include

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	kaflight.cc
TARGETS =       $(LIBDIR)libkaflight$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	kaflight$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

kaflight$(GL_EXT)$(BIN_EXT).o: ../include/kaflight.h ../include/input_device.h
kaflight$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h ../include/nps_device.h
kaflight$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h
