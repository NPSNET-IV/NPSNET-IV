# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
#OPT	=	-O2
#CFLAGS	=	$(OPT)

# You can locally define any symbols you need using the next line
DEFINES	= -DSS_INTERFACE 

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

SOURCES	=	jack_di_veh.cc jack_sailor_veh.cc
TARGETS =       $(LIBDIR)libjackdi$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	jack_di_veh$(GL_EXT)$(BIN_EXT).o \
		jack_sailor_veh$(GL_EXT)$(BIN_EXT).o


# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

jack_di_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h /usr/include/Performer/pfdu.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../include/jack_di_veh.h ../include/human_const.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../include/person.h ../include/human_constants.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../include/ground_veh.h ../include/vehicle.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../include/entity_const.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h ../include/weapon.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/draw.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../c2/include/highlight.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h ../include/jointmods.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/Jack/JackML/jacksoldier.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/Jack/JackML/jackPacket.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../include/formation.h ../include/signals.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/Jack/JackML/jack.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/appearance.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/disnetlib.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/netutil.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../database/include/terrain.h ../include/collision_const.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/manager.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/idunetlib.h
jack_di_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/idu.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h ../../apps/include/manager.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../include/jack_sailor_veh.h ../include/jack_di_veh.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../include/human_const.h ../include/person.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../include/human_constants.h ../include/ground_veh.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../include/vehicle.h ../include/entity_const.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../include/weapon.h ../../communication/include/pdu.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/draw.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../c2/include/highlight.h ../../c2/include/c2_const.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h ../include/jointmods.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/Jack/JackML/jacksoldier.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/Jack/JackML/jackPacket.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../include/formation.h ../include/signals.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/Jack/JackML/jack.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../include/ship_walk_veh.h ../include/ship_veh.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/idunetlib.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/idu.h ../include/casualties.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/appearance.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/disnetlib.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/netutil.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../../database/include/terrain.h
jack_sailor_veh$(GL_EXT)$(BIN_EXT).o: ../include/collision_const.h ../include/munitions.h
