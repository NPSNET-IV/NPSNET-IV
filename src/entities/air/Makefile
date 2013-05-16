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

SOURCES	=	aircraft_veh.cc fixed_wing_veh.cc real_rotary_wing_veh.cc \
		rotary_wing_veh.cc unmanned_veh.cc

TARGETS =       $(LIBDIR)libairveh$(GL_EXT)$(BIN_EXT).$(LIB_EXT)

OBJECTS	=	aircraft_veh$(GL_EXT)$(BIN_EXT).o \
		fixed_wing_veh$(GL_EXT)$(BIN_EXT).o \
		real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o \
		rotary_wing_veh$(GL_EXT)$(BIN_EXT).o \
		unmanned_veh$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../include/aircraft_veh.h ../include/vehicle.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../include/entity_const.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../include/weapon.h /usr/include/Performer/pf.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/draw.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../c2/include/highlight.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h /usr/include/Performer/opengl.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h ../../apps/include/macros.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../database/include/dirt_intersect.h ../include/entity.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../include/munitions.h ../include/collision_const.h
aircraft_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/interface_const.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../include/fixed_wing_veh.h ../include/aircraft_veh.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../include/vehicle.h ../include/entity_const.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../include/weapon.h /usr/include/Performer/pf.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/draw.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../c2/include/highlight.h ../../c2/include/c2_const.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h /usr/include/Performer/opengl.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../database/include/terrain.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/macros.h ../include/entity.h
fixed_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/interface_const.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../include/real_rotary_wing_veh.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../include/rotary_wing_veh.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../include/aircraft_veh.h ../include/vehicle.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../include/entity_const.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../include/weapon.h /usr/include/Performer/pf.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/draw.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../c2/include/highlight.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../database/include/terrain.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/macros.h ../include/entity.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/interface_const.h
real_rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fcs.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../include/rotary_wing_veh.h ../include/aircraft_veh.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../include/vehicle.h ../include/entity_const.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../include/weapon.h /usr/include/Performer/pf.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/draw.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../c2/include/highlight.h ../../c2/include/c2_const.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../database/include/terrain.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/macros.h ../include/entity.h
rotary_wing_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/interface_const.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../include/unmanned_veh.h ../include/aircraft_veh.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../include/vehicle.h ../include/entity_const.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../include/weapon.h /usr/include/Performer/pf.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/draw.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../c2/include/highlight.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h /usr/include/Performer/opengl.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../database/include/terrain.h ../../apps/include/macros.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../include/entity.h ../../interface/include/interface_const.h
unmanned_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/fontdef.h
