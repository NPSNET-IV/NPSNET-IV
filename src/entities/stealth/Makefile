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

SOURCES	=	stealth_veh.cc
TARGETS =       $(LIBDIR)libstealthveh$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	stealth_veh$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

stealth_veh$(GL_EXT)$(BIN_EXT).o: ../include/stealth_veh.h ../include/vehicle.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../include/entity_const.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h ../include/weapon.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h ../../communication/include/pdu.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/draw.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../c2/include/highlight.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../database/include/terrain.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/interface_const.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/iportClass.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/DI_comm.h ../../apps/include/manager.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/view.h ../include/diguy_veh.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../include/diguyTypeLookup.h ../../setup/include/imclass.h
stealth_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/transportClass.h
