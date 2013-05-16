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

SOURCES	=	person.cc person_input.cc formation.cc jointmods.cc signals.cc

TARGETS =       $(LIBDIR)libhuman$(GL_EXT)$(BIN_EXT).$(LIB_EXT)

OBJECTS	=	person$(GL_EXT)$(BIN_EXT).o \
		person_input$(GL_EXT)$(BIN_EXT).o \
		formation$(GL_EXT)$(BIN_EXT).o \
		jointmods$(GL_EXT)$(BIN_EXT).o \
		signals$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

person$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
person$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
person$(GL_EXT)$(BIN_EXT).o: ../../apps/include/manager.h
person$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h /usr/include/Performer/pf.h
person$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
person$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h
person$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
person$(GL_EXT)$(BIN_EXT).o: ../../interface/include/iportClass.h
person$(GL_EXT)$(BIN_EXT).o: ../../interface/include/DI_comm.h ../include/person.h
person$(GL_EXT)$(BIN_EXT).o: ../include/human_constants.h ../include/ground_veh.h
person$(GL_EXT)$(BIN_EXT).o: ../include/vehicle.h ../include/entity_const.h
person$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h ../include/weapon.h
person$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h ../../display/include/draw.h
person$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
person$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
person$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
person$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
person$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h ../../c2/include/highlight.h
person$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h ../../display/include/XCursorUtil.h
person$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
person$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
person$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
person$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
person$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
person$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
person$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
person$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
person$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
person$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
person$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
person$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h ../include/jointmods.h
person$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/Jack/JackML/jack.h
person$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/Jack/JackML/jacksoldier.h
person$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/Jack/JackML/jackPacket.h
person$(GL_EXT)$(BIN_EXT).o: ../include/human_const.h ../../apps/include/common_defs2.h
person$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h ../../apps/include/macros.h
person$(GL_EXT)$(BIN_EXT).o: ../../communication/include/appearance.h
person$(GL_EXT)$(BIN_EXT).o: ../../database/include/dirt_intersect.h
person$(GL_EXT)$(BIN_EXT).o: ../../communication/include/netutil.h
person$(GL_EXT)$(BIN_EXT).o: ../../communication/include/disnetlib.h ../include/entity.h
person$(GL_EXT)$(BIN_EXT).o: ../include/munitions.h ../include/collision_const.h
person$(GL_EXT)$(BIN_EXT).o: ../../interface/include/interface_const.h
person$(GL_EXT)$(BIN_EXT).o: ../../environment/include/environment_const.h
person_input$(GL_EXT)$(BIN_EXT).o: ../include/person.h ../include/human_constants.h
person_input$(GL_EXT)$(BIN_EXT).o: ../include/ground_veh.h ../include/vehicle.h
person_input$(GL_EXT)$(BIN_EXT).o: ../include/entity_const.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
person_input$(GL_EXT)$(BIN_EXT).o: ../include/weapon.h /usr/include/Performer/pf.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../display/include/draw.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
person_input$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../c2/include/highlight.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h
person_input$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h /usr/include/Performer/opengl.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h ../include/jointmods.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/appearance.h
person_input$(GL_EXT)$(BIN_EXT).o: ../include/human_const.h ../../apps/include/manager.h
person_input$(GL_EXT)$(BIN_EXT).o: ../include/jack_di_veh.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/Jack/JackML/jacksoldier.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/Jack/JackML/jackPacket.h
person_input$(GL_EXT)$(BIN_EXT).o: ../include/formation.h ../include/signals.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/DI_comm.h ../include/munitions.h
person_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/interface_const.h
formation$(GL_EXT)$(BIN_EXT).o: ../include/formation.h ../include/jointmods.h
jointmods$(GL_EXT)$(BIN_EXT).o: ../include/jointmods.h
signals$(GL_EXT)$(BIN_EXT).o: ../include/signals.h ../include/jointmods.h
