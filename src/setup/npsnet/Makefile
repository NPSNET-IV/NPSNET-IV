# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
OPT	=
#CFLAGS	=	$(OPT)

# You can locally define any symbols you need using the next line
#DEFINES	= -DDEBUG

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

SOURCES	=	read_config.cc
TARGETS =       $(LIBDIR)libnps_setup$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	read_config$(GL_EXT)$(BIN_EXT).o


# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

read_config$(GL_EXT)$(BIN_EXT).o: ../include/read_config.h ../../apps/include/common_defs.h
read_config$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pf.h ../../communication/include/pdu.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
read_config$(GL_EXT)$(BIN_EXT).o: ../include/setup_const.h ../../display/include/display_const.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
read_config$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h ../../c2/include/highlight.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../c2/include/c2_const.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h /usr/include/Performer/pr.h
read_config$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../entities/include/vehicle.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../entities/include/entity_const.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../entities/include/weapon.h ../../display/include/draw.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h ../include/fileio.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../database/include/database_const.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../media/include/media_const.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../communication/include/communication_const.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../interface/include/interface_const.h
read_config$(GL_EXT)$(BIN_EXT).o: ../../environment/include/environment_const.h
read_config$(GL_EXT)$(BIN_EXT).o: ../include/imstructs.h ../include/imclass.h
read_config$(GL_EXT)$(BIN_EXT).o: ../include/imdefaults.h
