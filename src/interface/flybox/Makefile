# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
#OPT	=	-O2 -w
#OPT	=	-g
#CFLAGS	=	$(OPT)

# You can locally define any symbols you need using the next line
#DEFINES	=

# You can define include directories your code needs here --
#    other than ../include
APPINCDIRS	=	-I/usr/include/Performer -I/usr/src/Performer/include

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	check_rev.cc convert.cc flybox.cc init_lv.cc open_lv.cc \
		pack.cc read_write.cc

TARGETS =       $(LIBDIR)libflybox$(GL_EXT)$(BIN_EXT).$(LIB_EXT)

OBJECTS	=	check_rev$(GL_EXT)$(BIN_EXT).o \
		convert$(GL_EXT)$(BIN_EXT).o \
		flybox$(GL_EXT)$(BIN_EXT).o \
		init_lv$(GL_EXT)$(BIN_EXT).o \
		open_lv$(GL_EXT)$(BIN_EXT).o \
		pack$(GL_EXT)$(BIN_EXT).o \
		read_write$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

check_rev$(GL_EXT)$(BIN_EXT).o: ../include/lv3.h
convert$(GL_EXT)$(BIN_EXT).o: ../include/lv3.h
flybox$(GL_EXT)$(BIN_EXT).o: ../include/flybox.h ../include/lv3.h ../include/input_device.h
flybox$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h ../include/nps_device.h
flybox$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h
init_lv$(GL_EXT)$(BIN_EXT).o: ../include/lv3.h
open_lv$(GL_EXT)$(BIN_EXT).o: ../include/lv3.h
pack$(GL_EXT)$(BIN_EXT).o: ../include/lv3.h
read_write$(GL_EXT)$(BIN_EXT).o: ../include/lv3.h
