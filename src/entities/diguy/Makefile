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
	-I../../../otherlibs/DI_Guy/include \
	-I/usr/include/Performer -I/usr/src/Performer/include

# You can define your own standard include files, if necessary
STDINCS	=\
	../../apps/include/common_defs.h \
	../../apps/include/common_defs2.h ../../apps/include/constants.h \
	../../apps/include/macros.h ../include/vehicle.h

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES =	diguy_veh.cc diguyTypeLookup.cc
TARGETS =       $(LIBDIR)libdiguy_veh$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	diguy_veh$(GL_EXT)$(BIN_EXT).o \
		diguyTypeLookup$(GL_EXT)$(BIN_EXT).o


# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# ***** ALWAYS list object file dependencies here

vehicle.o:	vehicle.cc $(INCDIR)vehicle.h

# ***********************************************************************

# DO NOT DELETE THIS LINE -- make depend depends on it.

diguy_veh$(GL_EXT)$(BIN_EXT).o: ../include/vehicle.h ../include/entity_const.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h ../include/weapon.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h ../../communication/include/pdu.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/draw.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../c2/include/highlight.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h ../../display/include/XCursorUtil.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h ../include/human_constants.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../include/diguy_veh.h ../include/diguyTypeLookup.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../setup/include/imclass.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/DI_Guy/include/libbdihml.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/DI_Guy/include/libhmlchar.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/DI_Guy/include/diguy.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h ../../apps/include/constants.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/macros.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../database/include/dirt_intersect.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../database/include/bbox.h ../../media/include/sound.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/disnetlib.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/netutil.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/manager.h ../../display/include/effects.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../include/entity.h ../include/munitions.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/appearance.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/interface_const.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/local_snd_code.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../include/collision_const.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/articulat.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../include/diApperance203.h ../../setup/include/imstructs.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../setup/include/imdefaults.h
diguy_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/communication_const.h
diguyTypeLookup$(GL_EXT)$(BIN_EXT).o: ../include/diguyTypeLookup.h
diguyTypeLookup$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/DI_Guy/include/libbdihml.h
diguyTypeLookup$(GL_EXT)$(BIN_EXT).o: ../../../otherlibs/DI_Guy/include/libhmlchar.h
