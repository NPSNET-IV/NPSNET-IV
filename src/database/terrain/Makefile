
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

SOURCES	=	dirt_intersect.cc \
		dirt_s1k.cc \
		dirt_flt.cc \
		dirt_load.cc \
		dirt_static.cc

TARGETS =       $(LIBDIR)libterrain$(GL_EXT)$(BIN_EXT).$(LIB_EXT)

OBJECTS	=	dirt_intersect$(GL_EXT)$(BIN_EXT).o \
		dirt_s1k$(GL_EXT)$(BIN_EXT).o \
		dirt_flt$(GL_EXT)$(BIN_EXT).o \
		dirt_load$(GL_EXT)$(BIN_EXT).o \
		dirt_static$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

dirt_intersect$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h /usr/include/Performer/pfutil.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../include/terrain.h ../../apps/include/common_defs2.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../c2/include/highlight.h ../../c2/include/c2_const.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h /usr/include/Performer/opengl.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../entities/include/vehicle.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../entities/include/entity_const.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../entities/include/weapon.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../display/include/draw.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../include/fltcallbacks.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfdb/pfflt.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../setup/include/fileio.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../entities/include/munitions.h ../include/bbox.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../include/database_const.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../entities/include/collision_const.h
dirt_intersect$(GL_EXT)$(BIN_EXT).o: ../../environment/include/environment_const.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h /usr/include/Performer/pfdb/pfflt.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h /usr/include/Performer/pfdu.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h ../../apps/include/constants.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h ../../c2/include/highlight.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h ../../entities/include/vehicle.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../entities/include/entity_const.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../entities/include/weapon.h ../../display/include/draw.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h ../include/fltcallbacks.h
dirt_flt$(GL_EXT)$(BIN_EXT).o: ../../setup/include/fileio.h
dirt_load$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h /usr/include/Performer/pfutil.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h ../../apps/include/constants.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../c2/include/highlight.h ../../c2/include/c2_const.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
dirt_load$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../entities/include/vehicle.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../entities/include/entity_const.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../entities/include/weapon.h ../../display/include/draw.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h ../../setup/include/fileio.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../include/bbox.h ../include/database_const.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../entities/include/collision_const.h ../include/dirt_flt.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../include/dirt_intersect.h ../../setup/include/imstructs.h
dirt_load$(GL_EXT)$(BIN_EXT).o: ../../setup/include/imclass.h
dirt_static$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h
dirt_static$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfdb/pfflt.h
dirt_static$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h /usr/include/Performer/pfdu.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../include/terrain.h ../../apps/include/common_defs2.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../c2/include/highlight.h ../../c2/include/c2_const.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
dirt_static$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../entities/include/vehicle.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../entities/include/entity_const.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../entities/include/weapon.h ../../display/include/draw.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h ../include/fltcallbacks.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../setup/include/fileio.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../entities/include/munitions.h ../include/bbox.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../include/database_const.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../entities/include/collision_const.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../environment/include/environment_const.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../include/loadBBoard.h ../../setup/include/imstructs.h
dirt_static$(GL_EXT)$(BIN_EXT).o: ../../setup/include/imclass.h
