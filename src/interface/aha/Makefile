# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
#OPT	=	-O2 -w
#OPT	=	-g
OPT	=
#CFLAGS	=	$(OPT)

# You can locally define any symbols you need using the next line
DEFINES	=	-DCOLLIDE

# You can define include directories your code needs here --
#    other than ../include
APPINCDIRS	=	-I/usr/include/Performer -I/usr/src/Performer/include

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	aha.cc
TARGETS =       $(LIBDIR)libaha$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	aha$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

aha$(GL_EXT)$(BIN_EXT).o: ../include/aha.h /usr/include/Performer/pf.h
aha$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h
