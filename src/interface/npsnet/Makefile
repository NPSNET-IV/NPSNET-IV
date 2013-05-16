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
	-I/usr/include/Performer -I/usr/src/Performer/include \
	-I../../../otherlibs/DVW/libdvw/include \
	-I../../../otherlibs/DVW/libdvw/libesm
#	-I../../../otherlibs/DVW/libdvw/protocol

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	nps_input.cc
TARGETS =       $(LIBDIR)libnps_interface$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	nps_input$(GL_EXT)$(BIN_EXT).o


# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

nps_input$(GL_EXT)$(BIN_EXT).o: ../include/nps_input.h /usr/include/Performer/pf.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h ../include/nps_device.h
nps_input$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../c2/include/highlight.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h ../include/hmdClass.h
nps_input$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h /usr/include/Performer/opengl.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../include/FastrakClass.h ../include/fakespaceClass.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../include/input_device.h ../include/lw_tcpip.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h ../include/lw-nps.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../include/input_manager.h ../../apps/include/manager.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../include/interface_const.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/communication_const.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h ../../apps/include/constants.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../entities/include/vehicle.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../entities/include/entity_const.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../entities/include/weapon.h ../../display/include/draw.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/time_funcs.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../environment/include/nps_smoke.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../environment/include/environment_const.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../display/include/effects.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../../entities/include/munitions.h ../include/fcs.h
nps_input$(GL_EXT)$(BIN_EXT).o: ../include/odt.h
