# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
OPT	=	-O2
#CFLAGS	=	$(OPT)

# You can locally define any symbols you need using the next line
#DEFINES	= -DSEARCH_PATH_TRACE

# You can define include directories your code needs here --
#    other than ../include
#APPINCDIRS	=

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	fileio.cc
TARGETS =       $(LIBDIR)libfileio$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	fileio$(BIN_EXT).o


# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

fileio$(BIN_EXT).o: ../include/fileio.h
