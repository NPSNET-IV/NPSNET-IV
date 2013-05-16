# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
#OPT	=	-O2
OPT	=	-g
#CFLAGS	=	$(OPT)

# You can locally define any symbols you need using the next line
DEFINES	=	-DDEBUG

# You can define include directories your code needs here --
#    other than ../include
APPINCDIRS      =	-I../../../otherlibs/ModSAF/include/protocol \
			-I../../../otherlibs/ModSAF/include/global \
			-I../../../otherlibs/ModSAF/include/libinc

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	po_net.c
TARGETS =       $(LIBDIR)libpo_net$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	po_net$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.
