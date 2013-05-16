# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
OPT	=	-O2
#OPT	=	-g
#CFLAGS	=	$(OPT)

# You can locally define any symbols you need using the next line
DEFINES	=	-DNDEBUG

# You can define include directories your code needs here --
#    other than ../include
APPINCDIRS	=	-I../../utilities/include

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	idunetlib.cc
TARGETS =       $(LIBDIR)libidunet$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	idunetlib$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

idunetlib$(BIN_EXT).o: ../include/idunetlib.h ../include/idu.h ../include/pdu.h
idunetlib$(BIN_EXT).o: ../include/envirvalues.h
