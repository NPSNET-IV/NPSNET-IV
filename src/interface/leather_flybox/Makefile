# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
#OPT	=	-O2 -w
#OPT	=	-g
OPT	=
#CFLAGS	=	$(OPT)

# You can locally define any symbols you need using the next line
#DEFINES	=	-DDEBUG -DSHOW_DATA -DTRACE -DSTATS
#DEFINES	=

# You can define include directories your code needs here --
#    other than ../include
APPINCDIRS	=	-I/usr/include/Performer -I/usr/src/Performer/include

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	leather_flybox.cc joystick.c fblib.c
TARGETS =       $(LIBDIR)libleather_flybox$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	leather_flybox$(GL_EXT)$(BIN_EXT).o \
		joystick$(GL_EXT)$(BIN_EXT).o \
		fblib$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

leather_flybox$(GL_EXT)$(BIN_EXT).o: ../include/leather_flybox.h ../include/input_device.h
leather_flybox$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h ../include/nps_device.h
leather_flybox$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../include/joystick.h
joystick$(GL_EXT)$(BIN_EXT).o: ../include/joystick.h ../include/fblib.h ../include/fb.h
fblib$(GL_EXT)$(BIN_EXT).o: ../include/fb.h ../include/fblib.h
