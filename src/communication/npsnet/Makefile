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
	-I../../../otherlibs/DVW/libdvw/include

#	-I../../../otherlibs/DVW/libdvw/libesm
#	-I../../../otherlibs/DVW/libdvw/protocol

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****
SOURCES	=	netutil.cc
TARGETS =       $(LIBDIR)libnps_comm$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	netutil$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

netutil$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h /usr/include/Performer/pfdb/pfflt.h
netutil$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfdu.h /usr/include/Performer/pfutil.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h ../../apps/include/constants.h
netutil$(GL_EXT)$(BIN_EXT).o: ../include/pdu.h ../include/envirvalues.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h ../include/rwstruct.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h ../../c2/include/highlight.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h ../include/identify.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
netutil$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h ../include/ip_co_sockcomm.h
netutil$(GL_EXT)$(BIN_EXT).o: ../include/ip_sockcomm.h ../include/sockcomm.h
netutil$(GL_EXT)$(BIN_EXT).o: ../include/co_sockcomm.h ../../interface/include/lw-nps.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../entities/include/vehicle.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../entities/include/entity_const.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../entities/include/weapon.h ../../display/include/draw.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h ../include/netutil.h
netutil$(GL_EXT)$(BIN_EXT).o: ../include/disnetlib.h ../../apps/include/macros.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../entities/include/munitions.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../database/include/fltcallbacks.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../display/include/effects.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../environment/include/nps_smoke.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../environment/include/environment_const.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../database/include/terrain.h ../../media/include/sound.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../entities/include/entity.h ../include/appearance.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../setup/include/fileio.h ../../entities/include/jack_di_veh.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../entities/include/human_const.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../entities/include/person.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../entities/include/human_constants.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../entities/include/ground_veh.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../entities/include/jointmods.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/Jack/JackML/jacksoldier.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/Jack/JackML/jackPacket.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../entities/include/formation.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../entities/include/signals.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../entities/include/diguy_veh.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../entities/include/diguyTypeLookup.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../setup/include/imclass.h ../../setup/include/imstructs.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../setup/include/imdefaults.h ../include/communication_const.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/Jack/JackML/jack.h
netutil$(GL_EXT)$(BIN_EXT).o: ../../entities/include/collision_const.h
netutil$(GL_EXT)$(BIN_EXT).o: ../include/local_snd_code.h
