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
	-I$(JACKML_INCDIR)  -I$(IPORT_INCDIR) -I$(MCAST_INCDIR)\
	-I/usr/include/Performer -I/usr/src/Performer/include \
	-I../../../otherlibs/DVW/libdvw/include

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	animations.cc cull.cc display_input.cc draw.cc effects.cc \
		intro.cc view.cc window.cc

TARGETS =       $(LIBDIR)libnps_display$(GL_EXT)$(BIN_EXT).$(LIB_EXT)

OBJECTS	=	animations$(GL_EXT)$(BIN_EXT).o \
		cull$(GL_EXT)$(BIN_EXT).o \
		display_input$(GL_EXT)$(BIN_EXT).o \
		draw$(GL_EXT)$(BIN_EXT).o \
		effects$(GL_EXT)$(BIN_EXT).o \
		intro$(GL_EXT)$(BIN_EXT).o \
		view$(GL_EXT)$(BIN_EXT).o \
		window$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

animations$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h /usr/include/Performer/pfdu.h
animations$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfdb/pfflt.h ../include/animations.h
animations$(GL_EXT)$(BIN_EXT).o: ../../setup/include/fileio.h
animations$(GL_EXT)$(BIN_EXT).o: ../../database/include/loadBBoard.h
animations$(GL_EXT)$(BIN_EXT).o: ../../setup/include/imdefaults.h
animations$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
animations$(GL_EXT)$(BIN_EXT).o: ../../communication/include/communication_const.h
animations$(GL_EXT)$(BIN_EXT).o: ../../setup/include/imstructs.h
animations$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
animations$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
cull$(GL_EXT)$(BIN_EXT).o: ../include/cull.h /usr/include/Performer/pf.h
cull$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h ../../apps/include/constants.h
cull$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
cull$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
cull$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
cull$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
cull$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h ../include/display_const.h
cull$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h /usr/include/Performer/pfutil.h
cull$(GL_EXT)$(BIN_EXT).o: ../../c2/include/highlight.h ../../c2/include/c2_const.h
cull$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h ../include/XCursorUtil.h
cull$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
cull$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
cull$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
cull$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
cull$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
cull$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
cull$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
cull$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
cull$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
cull$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
cull$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h ../../entities/include/vehicle.h
cull$(GL_EXT)$(BIN_EXT).o: ../../entities/include/entity_const.h
cull$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
cull$(GL_EXT)$(BIN_EXT).o: ../../entities/include/weapon.h ../include/draw.h
cull$(GL_EXT)$(BIN_EXT).o: ../include/display_types.h ../../interface/include/input_manager.h
cull$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
cull$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h ../../utilities/include/cpu.h
display_input$(GL_EXT)$(BIN_EXT).o: ../../apps/include/manager.h
display_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
display_input$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h
display_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
display_input$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h
display_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
display_input$(GL_EXT)$(BIN_EXT).o: ../include/display_input.h ../include/display_types.h
display_input$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
display_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
display_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
display_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
display_input$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h ../include/display_const.h
display_input$(GL_EXT)$(BIN_EXT).o: ../../c2/include/highlight.h ../../c2/include/c2_const.h
display_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
display_input$(GL_EXT)$(BIN_EXT).o: ../include/XCursorUtil.h ../../interface/include/hmdClass.h
display_input$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h /usr/include/Performer/opengl.h
display_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
display_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
display_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
display_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
display_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
display_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
display_input$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
display_input$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h ../include/draw.h
display_input$(GL_EXT)$(BIN_EXT).o: ../../entities/include/entity_const.h
display_input$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
draw$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../include/draw.h
draw$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h ../include/display_types.h
draw$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h ../../communication/include/pdu.h
draw$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
draw$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
draw$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h ../include/display_const.h
draw$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h ../../c2/include/highlight.h
draw$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h ../../communication/include/identify.h
draw$(GL_EXT)$(BIN_EXT).o: ../include/XCursorUtil.h ../../interface/include/hmdClass.h
draw$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h /usr/include/Performer/opengl.h
draw$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
draw$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
draw$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
draw$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
draw$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
draw$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
draw$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
draw$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
draw$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
draw$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h ../include/display_input.h
draw$(GL_EXT)$(BIN_EXT).o: ../../apps/include/manager.h ../../apps/include/common_defs2.h
draw$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h ../../entities/include/vehicle.h
draw$(GL_EXT)$(BIN_EXT).o: ../../entities/include/entity_const.h
draw$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
draw$(GL_EXT)$(BIN_EXT).o: ../../entities/include/weapon.h
draw$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
draw$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h
draw$(GL_EXT)$(BIN_EXT).o: ../../communication/include/appearance.h
draw$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input.h
draw$(GL_EXT)$(BIN_EXT).o: ../../interface/include/interface_const.h ../include/intro.h
draw$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/screen_dump.h ../include/fontdef.h
draw$(GL_EXT)$(BIN_EXT).o: ../../database/include/dbPagingCallback.h
draw$(GL_EXT)$(BIN_EXT).o: ../../database/include/dbDeletePageClass.h
draw$(GL_EXT)$(BIN_EXT).o: ../../database/include/tsg.h ../../database/include/tsgpf.h
draw$(GL_EXT)$(BIN_EXT).o: ../../database/include/tsgBuf.h
effects$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h /usr/include/Performer/pfdu.h
effects$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfdb/pfflt.h ../include/effects.h
effects$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h ../../apps/include/constants.h
effects$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
effects$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
effects$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
effects$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
effects$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h ../include/display_const.h
effects$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
effects$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../c2/include/highlight.h
effects$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h ../../communication/include/identify.h
effects$(GL_EXT)$(BIN_EXT).o: ../include/XCursorUtil.h ../../interface/include/hmdClass.h
effects$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h /usr/include/Performer/opengl.h
effects$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
effects$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
effects$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
effects$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
effects$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
effects$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
effects$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
effects$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
effects$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h ../../entities/include/vehicle.h
effects$(GL_EXT)$(BIN_EXT).o: ../../entities/include/entity_const.h
effects$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
effects$(GL_EXT)$(BIN_EXT).o: ../../entities/include/weapon.h ../include/draw.h
effects$(GL_EXT)$(BIN_EXT).o: ../include/display_types.h ../../interface/include/input_manager.h
effects$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
effects$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h ../../apps/include/macros.h
effects$(GL_EXT)$(BIN_EXT).o: ../include/animations.h ../../database/include/fltcallbacks.h
effects$(GL_EXT)$(BIN_EXT).o: ../../database/include/terrain.h
effects$(GL_EXT)$(BIN_EXT).o: ../../entities/include/collision_const.h
effects$(GL_EXT)$(BIN_EXT).o: ../../setup/include/imstructs.h ../../setup/include/imclass.h
effects$(GL_EXT)$(BIN_EXT).o: ../../setup/include/imextern.h
intro$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h ../../database/include/texXLoad.h
intro$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h ../../apps/include/constants.h
intro$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
intro$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
intro$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
intro$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
intro$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h ../include/display_const.h
intro$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h /usr/include/Performer/pfutil.h
intro$(GL_EXT)$(BIN_EXT).o: ../../c2/include/highlight.h ../../c2/include/c2_const.h
intro$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h ../include/XCursorUtil.h
intro$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
intro$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
intro$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
intro$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
intro$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
intro$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
intro$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
intro$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
intro$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
intro$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
intro$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h ../../entities/include/vehicle.h
intro$(GL_EXT)$(BIN_EXT).o: ../../entities/include/entity_const.h
intro$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
intro$(GL_EXT)$(BIN_EXT).o: ../../entities/include/weapon.h ../include/draw.h
intro$(GL_EXT)$(BIN_EXT).o: ../include/display_types.h ../../interface/include/input_manager.h
intro$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
intro$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h ../include/intro.h
view$(GL_EXT)$(BIN_EXT).o: ../include/view.h /usr/include/Performer/pf.h
view$(GL_EXT)$(BIN_EXT).o: ../include/display_types.h ../../apps/include/common_defs.h
view$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
view$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
view$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
view$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h ../include/display_const.h
view$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h /usr/include/Performer/pfutil.h
view$(GL_EXT)$(BIN_EXT).o: ../../c2/include/highlight.h ../../c2/include/c2_const.h
view$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h ../include/XCursorUtil.h
view$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
view$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
view$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
view$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
view$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
view$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
view$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
view$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
view$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
view$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
view$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
view$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h ../../apps/include/macros.h
view$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h ../../apps/include/common_defs2.h
view$(GL_EXT)$(BIN_EXT).o: ../../entities/include/vehicle.h
view$(GL_EXT)$(BIN_EXT).o: ../../entities/include/entity_const.h
view$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
view$(GL_EXT)$(BIN_EXT).o: ../../entities/include/weapon.h ../include/draw.h
view$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
view$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h
view$(GL_EXT)$(BIN_EXT).o: ../../communication/include/appearance.h
view$(GL_EXT)$(BIN_EXT).o: ../../entities/include/collision_const.h
view$(GL_EXT)$(BIN_EXT).o: ../../entities/include/ship_veh.h
view$(GL_EXT)$(BIN_EXT).o: ../../communication/include/idunetlib.h
view$(GL_EXT)$(BIN_EXT).o: ../../communication/include/idu.h
window$(GL_EXT)$(BIN_EXT).o: ../include/window.h /usr/include/Performer/pf.h
window$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h ../../apps/include/constants.h
window$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
window$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
window$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
window$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
window$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h ../include/display_const.h
window$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
window$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../c2/include/highlight.h
window$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h ../../communication/include/identify.h
window$(GL_EXT)$(BIN_EXT).o: ../include/XCursorUtil.h ../../interface/include/hmdClass.h
window$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h /usr/include/Performer/opengl.h
window$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
window$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
window$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
window$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
window$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
window$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
window$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
window$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
window$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h ../../entities/include/vehicle.h
window$(GL_EXT)$(BIN_EXT).o: ../../entities/include/entity_const.h
window$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
window$(GL_EXT)$(BIN_EXT).o: ../../entities/include/weapon.h ../include/draw.h
window$(GL_EXT)$(BIN_EXT).o: ../include/display_types.h ../../interface/include/input_manager.h
window$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
window$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h ../../apps/include/manager.h
window$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/cpu.h ../include/fontdef.h
window$(GL_EXT)$(BIN_EXT).o: ../include/intro.h
