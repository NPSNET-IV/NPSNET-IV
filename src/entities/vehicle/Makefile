# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

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

# You can define your own standard include files, if necessary
STDINCS	=\
	../../apps/include/common_defs.h \
	../../apps/include/common_defs2.h ../../apps/include/constants.h \
	../../apps/include/macros.h ../include/vehicle.h

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES =	radar.cc vehicle.cc vehicle_input.cc
TARGETS =       $(LIBDIR)libvehicle$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	radar$(GL_EXT)$(BIN_EXT).o \
		vehicle$(GL_EXT)$(BIN_EXT).o \
		vehicle_input$(GL_EXT)$(BIN_EXT).o


# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# ***** ALWAYS list object file dependencies here

vehicle.o:	vehicle.cc $(INCDIR)vehicle.h

# ***********************************************************************

# DO NOT DELETE THIS LINE -- make depend depends on it.

radar$(GL_EXT)$(BIN_EXT).o: ../include/radar.h ../../apps/include/common_defs.h
radar$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h ../../communication/include/pdu.h
radar$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
radar$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
radar$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
radar$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
radar$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h /usr/include/Performer/pfutil.h
radar$(GL_EXT)$(BIN_EXT).o: ../../c2/include/highlight.h ../../c2/include/c2_const.h
radar$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
radar$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
radar$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
radar$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
radar$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
radar$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
radar$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
radar$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
radar$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
radar$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
radar$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
radar$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
radar$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h ../include/entity_const.h
radar$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
radar$(GL_EXT)$(BIN_EXT).o: ../../communication/include/appearance.h
radar$(GL_EXT)$(BIN_EXT).o: ../../display/include/fontdef.h ../../setup/include/imstructs.h
radar$(GL_EXT)$(BIN_EXT).o: ../../setup/include/imclass.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../include/vehicle.h ../include/entity_const.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h ../include/weapon.h
vehicle$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h ../../communication/include/pdu.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../display/include/draw.h ../../display/include/display_types.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
vehicle$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../c2/include/highlight.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h ../../display/include/XCursorUtil.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
vehicle$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../communication/include/disnetlib.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../apps/include/manager.h ../../apps/include/common_defs2.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h ../../apps/include/macros.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../communication/include/appearance.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../database/include/terrain.h ../include/entity.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../media/include/sound.h ../../display/include/effects.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../environment/include/nps_smoke.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../environment/include/environment_const.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../include/munitions.h ../../communication/include/netutil.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../database/include/bbox.h ../../setup/include/read_config.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../include/radar.h ../../display/include/fontdef.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../include/collision_const.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../interface/include/interface_const.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../communication/include/local_snd_code.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../communication/include/nps_converter.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/stdtypes.h
vehicle$(GL_EXT)$(BIN_EXT).o: ../../communication/include/articulat.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../apps/include/manager.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../c2/include/highlight.h ../../c2/include/c2_const.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h /usr/include/Performer/opengl.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h ../include/vehicle.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../include/entity_const.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../include/weapon.h ../../display/include/draw.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h ../include/munitions.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../database/include/terrain.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/interface_const.h
vehicle_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/keyboard.h
