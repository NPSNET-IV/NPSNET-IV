# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
#OPT	=	-O2 -w
#OPT	=	-g
OPT	=
CFLAGS	=	$(OPT) -ansi

# You can locally define any symbols you need using the next line
#DEFINES	=

# You can define include directories your code needs here --
#    other than ../include
APPINCDIRS	=

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	birdlib.cc
TARGETS =       $(LIBDIR)libbird$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	birdlib$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

birdlib$(GL_EXT)$(BIN_EXT).o: ../include/birdlib.h
