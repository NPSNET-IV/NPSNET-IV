# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
#OPT	=	-O2
#CFLAGS	=	$(OPT)

# You can locally define any symbols you need using the next line
#DEFINES	=

# You can define include directories your code needs here --
#    other than ../include
APPINCDIRS	=	-I/usr/include/Performer

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	tsgRBuf.c tsgROpen.c tsgRTile.c tsgBuf.c tsgFTile.c \
		tsgMode.c tsgRClose.c tsgRPrint.c
TARGETS =       $(LIBDIR)libtsg$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	tsgRBuf$(GL_EXT)$(BIN_EXT).o \
		tsgROpen$(GL_EXT)$(BIN_EXT).o \
		tsgRTile$(GL_EXT)$(BIN_EXT).o \
		tsgBuf$(GL_EXT)$(BIN_EXT).o \
		tsgFTile$(GL_EXT)$(BIN_EXT).o \
		tsgMode$(GL_EXT)$(BIN_EXT).o \
		tsgRClose$(GL_EXT)$(BIN_EXT).o \
		tsgRPrint$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

tsgRBuf$(GL_EXT)$(BIN_EXT).o: ../include/tsgpf.h ../include/tsg.h ../include/tsgBuf.h
tsgROpen$(GL_EXT)$(BIN_EXT).o: ../include/tsgpf.h ../include/tsg.h ../include/tsgBuf.h
tsgRTile$(GL_EXT)$(BIN_EXT).o: ../include/tsgpf.h ../include/tsg.h ../include/tsgBuf.h
tsgBuf$(GL_EXT)$(BIN_EXT).o: ../include/tsgpf.h ../include/tsg.h ../include/tsgBuf.h
tsgFTile$(GL_EXT)$(BIN_EXT).o: ../include/tsgpf.h ../include/tsg.h ../include/tsgBuf.h
tsgMode$(GL_EXT)$(BIN_EXT).o: ../include/tsg.h
tsgRClose$(GL_EXT)$(BIN_EXT).o: ../include/tsgpf.h ../include/tsg.h ../include/tsgBuf.h
tsgRPrint$(GL_EXT)$(BIN_EXT).o: ../include/tsgpf.h ../include/tsg.h ../include/tsgBuf.h
