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
APPINCDIRS	=	-I../../apps/include -I../../c2/include \
	-I../../communication/include -I../../database/include \
	-I../../display/include -I../../entities/include \
	-I../../environment/include -I../../interface/include \
	-I../../media/include -I../../setup/include -I../../utilities/include \
	-I$(JACKML_INCDIR) -I$(IPORT_INCDIR) -I$(MCAST_INCDIR) \
	-I/usr/include/Performer -I/usr/src/Performer/include

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	fltcallbacks.cc loadBBoard.cc
TARGETS =       $(LIBDIR)libloaders$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	fltcallbacks$(GL_EXT)$(BIN_EXT).o \
		loadBBoard$(GL_EXT)$(BIN_EXT).o \
		texXLoad$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../include/fltcallbacks.h /usr/include/Performer/pf.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfdb/pfflt.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../c2/include/highlight.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h /usr/include/Performer/opengl.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../entities/include/vehicle.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../entities/include/entity_const.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../entities/include/weapon.h ../../display/include/draw.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h
fltcallbacks$(GL_EXT)$(BIN_EXT).o: ../../setup/include/fileio.h
loadBBoard$(GL_EXT)$(BIN_EXT).o: ../include/loadBBoard.h
