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

SOURCES	=	submersible_veh.cc Hmatrix.cc Quaternion.cc RigidBody.cc \
                UUVbody.cc Vector3D.cc
TARGETS =       $(LIBDIR)libsubsurfaceveh$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	submersible_veh$(GL_EXT)$(BIN_EXT).o \
		Hmatrix$(GL_EXT)$(BIN_EXT).o \
		Quaternion$(GL_EXT)$(BIN_EXT).o \
		RigidBody$(GL_EXT)$(BIN_EXT).o \
                UUVbody$(GL_EXT)$(BIN_EXT).o \
		Vector3D$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

submersible_veh$(GL_EXT)$(BIN_EXT).o: ../include/submersible_veh.h ../include/vehicle.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../include/entity_const.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/conversion_const.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../include/weapon.h /usr/include/Performer/pf.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/identify.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/draw.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_types.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/rwstruct.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../setup/include/setup_const.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/display_const.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/nps_device.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfutil.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../c2/include/highlight.h ../../c2/include/c2_const.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../display/include/XCursorUtil.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/hmdClass.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/FastrakClass.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/fakespaceClass.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_device.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw_tcpip.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_co_sockcomm.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/ip_sockcomm.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/sockcomm.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/co_sockcomm.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/lw-nps.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/input_manager.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/automateClass.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../utilities/include/infoClass.h ../include/UUVmodel.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../include/SUBmodel.h ../include/RigidBody.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../include/Hmatrix.h ../include/Quaternion.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../include/Vector3D.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/idunetlib.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/idu.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/common_defs2.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/constants.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../database/include/terrain.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/netutil.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../communication/include/disnetlib.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../apps/include/macros.h ../include/munitions.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../interface/include/interface_const.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../include/collision_const.h ../include/entity.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../environment/include/lake.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pfdb/pfflt.h
submersible_veh$(GL_EXT)$(BIN_EXT).o: ../../environment/include/water.h
Hmatrix$(GL_EXT)$(BIN_EXT).o: ../include/Hmatrix.h ../include/Quaternion.h ../include/Vector3D.h
Quaternion$(GL_EXT)$(BIN_EXT).o: ../include/Quaternion.h ../include/Vector3D.h
RigidBody$(GL_EXT)$(BIN_EXT).o: ../include/RigidBody.h ../include/Hmatrix.h
RigidBody$(GL_EXT)$(BIN_EXT).o: ../include/Quaternion.h ../include/Vector3D.h
UUVbody$(GL_EXT)$(BIN_EXT).o: ../include/UUVmodel.h ../include/SUBmodel.h ../include/RigidBody.h
UUVbody$(GL_EXT)$(BIN_EXT).o: ../include/Hmatrix.h ../include/Quaternion.h ../include/Vector3D.h
Vector3D$(GL_EXT)$(BIN_EXT).o: ../include/Vector3D.h
