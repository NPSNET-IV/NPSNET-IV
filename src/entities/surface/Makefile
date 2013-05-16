# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
#OPT	=	-O2
#CFLAGS	=	$(OPT)

# You can locally define any symbols you need using the next line
DEFINES	= 

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

SOURCES	=	ship_veh.cc ship_veh_input.cc ship_walk_veh.cc \
		pvs.cc object.cc picking.cc matrix.cc casualties.cc
TARGETS =       $(LIBDIR)libsurfaceveh$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	ship_veh$(GL_EXT)$(BIN_EXT).o \
		ship_veh_input$(GL_EXT)$(BIN_EXT).o \
		ship_walk_veh$(GL_EXT)$(BIN_EXT).o \
		pvs$(GL_EXT)$(BIN_EXT).o \
		object$(GL_EXT)$(BIN_EXT).o \
		picking$(GL_EXT)$(BIN_EXT).o \
		matrix$(GL_EXT)$(BIN_EXT).o \
		casualties$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

ship_veh$(GL_EXT)$(BIN_EXT).o: ../include/ship_veh.h ../include/vehicle.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../include/entity_const.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h ../include/weapon.h
ship_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h ../../communication/include/pdu.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/draw.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
ship_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../c2/include/highlight.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h ../../display/include/XCursorUtil.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
ship_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/idunetlib.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/idu.h ../include/person.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../include/human_constants.h ../include/ground_veh.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../include/jointmods.h ../../apps/include/common_defs2.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/appearance.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../media/include/sound.h ../../display/include/effects.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../environment/include/nps_smoke.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../environment/include/environment_const.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/netutil.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/disnetlib.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../database/include/terrain.h ../include/munitions.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/interface_const.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../include/collision_const.h
ship_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/local_snd_code.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../apps/include/manager.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../c2/include/highlight.h ../../c2/include/c2_const.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h /usr/include/Performer/opengl.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h ../include/vehicle.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../include/entity_const.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../include/weapon.h ../../display/include/draw.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h ../include/ship_veh.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/idunetlib.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/idu.h ../include/munitions.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../database/include/terrain.h
ship_veh_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/interface_const.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h /usr/include/Performer/pf.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h ../../communication/include/pdu.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h /usr/include/Performer/pfutil.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../c2/include/highlight.h ../../c2/include/c2_const.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h ../../interface/include/hmdClass.h
pvs$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h /usr/include/Performer/opengl.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h ../include/vehicle.h
pvs$(GL_EXT)$(BIN_EXT).o: ../include/entity_const.h ../../utilities/include/conversion_const.h
pvs$(GL_EXT)$(BIN_EXT).o: ../include/weapon.h ../../display/include/draw.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
pvs$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h ../../apps/include/macros.h
pvs$(GL_EXT)$(BIN_EXT).o: ../include/pvs.h picking.h ../include/collision_const.h
object$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfdu.h /usr/include/Performer/pfdb/pfflt.h
object$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/prmath.h /usr/include/Performer/pfutil.h
object$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h /usr/include/Performer/pf.h
object$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h ../../communication/include/pdu.h
object$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
object$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
object$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
object$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
object$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
object$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h ../../c2/include/highlight.h
object$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h ../../communication/include/identify.h
object$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
object$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
object$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
object$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
object$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
object$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
object$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
object$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
object$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
object$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
object$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
object$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h ../include/vehicle.h
object$(GL_EXT)$(BIN_EXT).o: ../include/entity_const.h
object$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h ../include/weapon.h
object$(GL_EXT)$(BIN_EXT).o: ../../display/include/draw.h ../../display/include/display_types.h
object$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
object$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
object$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h ../include/object.h
object$(GL_EXT)$(BIN_EXT).o: ship_constants.h ../include/pvs.h ../include/collision_const.h
picking$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../apps/include/manager.h
picking$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
picking$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h ../../interface/include/nps_device.h
picking$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
picking$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h ../../apps/include/constants.h
picking$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
picking$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
picking$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
picking$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
picking$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
picking$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h ../../c2/include/highlight.h
picking$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h ../../communication/include/identify.h
picking$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
picking$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
picking$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
picking$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
picking$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
picking$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
picking$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
picking$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
picking$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
picking$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
picking$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h ../include/vehicle.h
picking$(GL_EXT)$(BIN_EXT).o: ../include/entity_const.h
picking$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h ../include/weapon.h
picking$(GL_EXT)$(BIN_EXT).o: ../../display/include/draw.h ../../display/include/display_types.h
picking$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
picking$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h
picking$(GL_EXT)$(BIN_EXT).o: ../../interface/include/interface_const.h picking.h
matrix$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/prmath.h /usr/include/Performer/pfutil.h
matrix$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfdb/pfflt.h matrix.h
casualties$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h /usr/include/Performer/pf.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../c2/include/highlight.h ../../c2/include/c2_const.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
casualties$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h ../include/vehicle.h
casualties$(GL_EXT)$(BIN_EXT).o: ../include/entity_const.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h ../include/weapon.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../display/include/draw.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../display/include/effects.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../environment/include/nps_smoke.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../environment/include/environment_const.h
casualties$(GL_EXT)$(BIN_EXT).o: ../../environment/include/timeofday.h ../include/casualties.h
