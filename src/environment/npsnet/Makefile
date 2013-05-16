# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
#OPT    =	-O2
#CFLAGS =	$(OPT)

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
#       -I../../../otherlibs/DVW/libdvw/protocol

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES =	environ_input.cc nps_smoke.cc timeofday.cc lake.cc
TARGETS =	$(LIBDIR)libnps_environ$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS =	environ_input$(GL_EXT)$(BIN_EXT).o \
		nps_smoke$(GL_EXT)$(BIN_EXT).o \
		timeofday$(GL_EXT)$(BIN_EXT).o \
		lake$(GL_EXT)$(BIN_EXT).o


# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

environ_input$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h
environ_input$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h ../../apps/include/constants.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
environ_input$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../c2/include/highlight.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h
environ_input$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h /usr/include/Performer/opengl.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../entities/include/vehicle.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../entities/include/entity_const.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../entities/include/weapon.h ../../display/include/draw.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../include/environ_input.h ../include/environment_const.h
environ_input$(GL_EXT)$(BIN_EXT).o: ../include/timeofday.h ../../apps/include/manager.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h /usr/include/Performer/pfdb/pfflt.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../include/nps_smoke.h ../include/environment_const.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h ../../apps/include/constants.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../c2/include/highlight.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../entities/include/vehicle.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../entities/include/entity_const.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../entities/include/weapon.h ../../display/include/draw.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../database/include/fltcallbacks.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../database/include/terrain.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../database/include/database_const.h
nps_smoke$(GL_EXT)$(BIN_EXT).o: ../../entities/include/collision_const.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../include/timeofday.h ../../apps/include/common_defs.h
timeofday$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h ../../communication/include/pdu.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
timeofday$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../c2/include/highlight.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
timeofday$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h ../include/environment_const.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h ../../apps/include/constants.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../entities/include/vehicle.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../entities/include/entity_const.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../entities/include/weapon.h ../../display/include/draw.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
timeofday$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h
lake$(GL_EXT)$(BIN_EXT).o: ../../database/include/terrain.h /usr/include/Performer/pf.h
lake$(GL_EXT)$(BIN_EXT).o: ../../entities/include/collision_const.h ../include/lake.h
lake$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfdb/pfflt.h ../include/water.h
lake$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h ../../communication/include/pdu.h
lake$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
lake$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
lake$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
lake$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
lake$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h /usr/include/Performer/pfutil.h
lake$(GL_EXT)$(BIN_EXT).o: ../../c2/include/highlight.h ../../c2/include/c2_const.h
lake$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
lake$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
lake$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
lake$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
lake$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
lake$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
lake$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
lake$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
lake$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
lake$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
lake$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
lake$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
lake$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
