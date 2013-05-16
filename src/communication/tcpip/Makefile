# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
#OPT	=	-O2 -w
CFLAGS	=	$(OPT) -ansi

# You can locally define any symbols you need using the next line
#DEFINES	=

# You can define include directories your code needs here --
#    other than ../include
APPINCDIRS	=

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	tcpip.c
TARGETS =       $(LIBDIR)libtcpip$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	tcpip$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

tcpip$(BIN_EXT).o: ../include/tcpip.h
