# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
#OPT	=	-O2
#CFLAGS	=	$(OPT)

# You can locally define any symbols you need with the following line
#DEFINES	=

# You can define include directories your code needs here --
#    other than ../include
APPINCDIRS	=	-I/usr/include/Performer -I/usr/src/Performer/include \
			-I../../apps/include \
			-I../../setup/include \
			-I../../display/include \
			-I../../interface/include \
			-I../../communication/include \
			-I../../database/include \
			-I../../entities/include \
			-I../../utilities/include \
			-I../../../otherlibs/ModSAF/nps_include \
			-I../../../otherlibs/ModSAF/include/protocol \
			-I../../../otherlibs/ModSAF/include/global \
			-I../../../otherlibs/ModSAF/include/libinc

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	ball_equa.cc highlight.cc line_build.cc \
		menu.cc menu_funcs.cc picking.cc po_ball.cc \
		po_ball_funcs.cc po_build.cc po_funcs.cc \
		po_interface.cc po_meas.cc

TARGETS =       $(LIBDIR)libnps_c2$(GL_EXT)$(BIN_EXT).$(LIB_EXT)

OBJECTS	=	ball_equa$(GL_EXT)$(BIN_EXT).o \
		highlight$(GL_EXT)$(BIN_EXT).o \
		line_build$(GL_EXT)$(BIN_EXT).o \
		menu$(GL_EXT)$(BIN_EXT).o \
		menu_funcs$(GL_EXT)$(BIN_EXT).o \
		picking$(GL_EXT)$(BIN_EXT).o \
		po_ball$(GL_EXT)$(BIN_EXT).o \
		po_ball_funcs$(GL_EXT)$(BIN_EXT).o \
		po_build$(GL_EXT)$(BIN_EXT).o \
		po_funcs$(GL_EXT)$(BIN_EXT).o \
		po_interface$(GL_EXT)$(BIN_EXT).o \
		po_meas$(GL_EXT)$(BIN_EXT).o


# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.
