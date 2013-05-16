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
			-I../../communication/include -I../../entities/include

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	FastrakClass.cc
TARGETS =       $(LIBDIR)libfastrak$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	FastrakClass$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

FastrakClass$(GL_EXT)$(BIN_EXT).o: ../include/FastrakClass.h
