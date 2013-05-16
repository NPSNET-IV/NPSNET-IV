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

SOURCES	=	entity.cc munitions.cc weapon.cc
TARGETS =       $(LIBDIR)libnps_entity$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	entity$(GL_EXT)$(BIN_EXT).o \
		munitions$(GL_EXT)$(BIN_EXT).o \
		weapon$(GL_EXT)$(BIN_EXT).o


# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

entity$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h ../include/entity.h
entity$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h ../../apps/include/constants.h
entity$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
entity$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
entity$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
entity$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
entity$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
entity$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
entity$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
entity$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../c2/include/highlight.h
entity$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h ../../communication/include/identify.h
entity$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
entity$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
entity$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
entity$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
entity$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
entity$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
entity$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
entity$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
entity$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
entity$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
entity$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
entity$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h ../include/vehicle.h
entity$(GL_EXT)$(BIN_EXT).o: ../include/entity_const.h
entity$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h ../include/weapon.h
entity$(GL_EXT)$(BIN_EXT).o: ../../display/include/draw.h ../../display/include/display_types.h
entity$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
entity$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
entity$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h
entity$(GL_EXT)$(BIN_EXT).o: ../../communication/include/appearance.h
entity$(GL_EXT)$(BIN_EXT).o: ../../database/include/terrain.h
entity$(GL_EXT)$(BIN_EXT).o: ../../communication/include/netutil.h
entity$(GL_EXT)$(BIN_EXT).o: ../../communication/include/disnetlib.h ../include/aircraft_veh.h
entity$(GL_EXT)$(BIN_EXT).o: ../include/fixed_wing_veh.h ../include/ground_veh.h
entity$(GL_EXT)$(BIN_EXT).o: ../include/jack_di_veh.h ../include/human_const.h
entity$(GL_EXT)$(BIN_EXT).o: ../include/person.h ../include/human_constants.h
entity$(GL_EXT)$(BIN_EXT).o: ../include/jointmods.h ../../../otherlibs/Jack/JackML/jacksoldier.h
entity$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/Jack/JackML/jackPacket.h ../include/formation.h
entity$(GL_EXT)$(BIN_EXT).o: ../include/signals.h ../include/jack_sailor_veh.h
entity$(GL_EXT)$(BIN_EXT).o: ../include/diguy_veh.h ../include/diguyTypeLookup.h
entity$(GL_EXT)$(BIN_EXT).o: ../../setup/include/imclass.h ../include/rotary_wing_veh.h
entity$(GL_EXT)$(BIN_EXT).o: ../include/real_rotary_wing_veh.h ../include/ship_veh.h
entity$(GL_EXT)$(BIN_EXT).o: ../../communication/include/idunetlib.h
entity$(GL_EXT)$(BIN_EXT).o: ../../communication/include/idu.h ../include/stealth_veh.h
entity$(GL_EXT)$(BIN_EXT).o: ../include/submersible_veh.h ../include/UUVmodel.h
entity$(GL_EXT)$(BIN_EXT).o: ../include/SUBmodel.h ../include/RigidBody.h ../include/Hmatrix.h
entity$(GL_EXT)$(BIN_EXT).o: ../include/Quaternion.h ../include/Vector3D.h
entity$(GL_EXT)$(BIN_EXT).o: ../include/unmanned_veh.h ../../interface/include/interface_const.h
munitions$(GL_EXT)$(BIN_EXT).o: ../include/munitions.h /usr/include/Performer/pf.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
munitions$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../c2/include/highlight.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
munitions$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h ../../display/include/draw.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../communication/include/disnetlib.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h ../../apps/include/constants.h
munitions$(GL_EXT)$(BIN_EXT).o: ../include/vehicle.h ../include/entity_const.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h ../include/weapon.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../communication/include/appearance.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../display/include/effects.h ../../database/include/terrain.h
munitions$(GL_EXT)$(BIN_EXT).o: ../include/jack_di_veh.h ../include/human_const.h
munitions$(GL_EXT)$(BIN_EXT).o: ../include/person.h ../include/human_constants.h
munitions$(GL_EXT)$(BIN_EXT).o: ../include/ground_veh.h ../include/jointmods.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/Jack/JackML/jacksoldier.h
munitions$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/Jack/JackML/jackPacket.h
munitions$(GL_EXT)$(BIN_EXT).o: ../include/formation.h ../include/signals.h
munitions$(GL_EXT)$(BIN_EXT).o: ../include/collision_const.h
weapon$(GL_EXT)$(BIN_EXT).o: ../include/weapon.h /usr/include/Performer/pf.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h ../../display/include/draw.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
weapon$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../c2/include/highlight.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h ../../display/include/XCursorUtil.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
weapon$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../communication/include/disnetlib.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h ../../apps/include/constants.h
weapon$(GL_EXT)$(BIN_EXT).o: ../include/vehicle.h ../include/entity_const.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h ../../apps/include/macros.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../display/include/effects.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../environment/include/nps_smoke.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../environment/include/environment_const.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../media/include/sound.h ../../database/include/terrain.h
weapon$(GL_EXT)$(BIN_EXT).o: ../include/munitions.h ../../communication/include/appearance.h
weapon$(GL_EXT)$(BIN_EXT).o: ../include/jack_di_veh.h ../include/human_const.h
weapon$(GL_EXT)$(BIN_EXT).o: ../include/person.h ../include/human_constants.h
weapon$(GL_EXT)$(BIN_EXT).o: ../include/ground_veh.h ../include/jointmods.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/Jack/JackML/jacksoldier.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/Jack/JackML/jackPacket.h ../include/formation.h
weapon$(GL_EXT)$(BIN_EXT).o: ../include/signals.h ../include/collision_const.h
weapon$(GL_EXT)$(BIN_EXT).o: ../../display/include/view.h
