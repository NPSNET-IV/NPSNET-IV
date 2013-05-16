# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
#OPT	=	-O2 -w
#OPT	=	-g
#CFLAGS	=	$(OPT)

# You can locally define any symbols you need using the next line
#DEFINES	=	-DDEBUG -DSHOW_DATA -DTRACE -DSTATS -DVTRACE

# You can define include directories your code needs here --
#    other than ../include
APPINCDIRS	=	-I/usr/include/Performer -I/usr/src/Performer/include \
			-I../../communication/include -I../../entities/include

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	ArmClass.cc LeftArmClass.cc RightArmClass.cc \
		RifleClass.cc UpperBodyClass.cc 
TARGETS =	$(LIBDIR)libupperbody$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	ArmClass$(GL_EXT)$(BIN_EXT).o \
		LeftArmClass$(GL_EXT)$(BIN_EXT).o \
		RightArmClass$(GL_EXT)$(BIN_EXT).o \
		RifleClass$(GL_EXT)$(BIN_EXT).o \
		UpperBodyClass$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

ArmClass$(GL_EXT)$(BIN_EXT).o: ../include/ArmClass.h
LeftArmClass$(GL_EXT)$(BIN_EXT).o: ../../entities/include/jointmods.h ../include/ArmClass.h
LeftArmClass$(GL_EXT)$(BIN_EXT).o: ../include/LeftArmClass.h
RightArmClass$(GL_EXT)$(BIN_EXT).o: ../../entities/include/jointmods.h ../include/ArmClass.h
RightArmClass$(GL_EXT)$(BIN_EXT).o: ../include/RightArmClass.h
RifleClass$(GL_EXT)$(BIN_EXT).o: ../../entities/include/jointmods.h ../include/RifleClass.h
UpperBodyClass$(GL_EXT)$(BIN_EXT).o: ../include/FastrakClass.h ../include/UpperBodyClass.h
UpperBodyClass$(GL_EXT)$(BIN_EXT).o: ../include/input_device.h /usr/include/Performer/pf.h
UpperBodyClass$(GL_EXT)$(BIN_EXT).o: ../include/nps_device.h /usr/include/Performer/pfutil.h
UpperBodyClass$(GL_EXT)$(BIN_EXT).o: ../include/RifleClass.h ../include/ArmClass.h
UpperBodyClass$(GL_EXT)$(BIN_EXT).o: ../include/RightArmClass.h ../include/LeftArmClass.h
UpperBodyClass$(GL_EXT)$(BIN_EXT).o: ../../entities/include/jointmods.h
UpperBodyClass$(GL_EXT)$(BIN_EXT).o: ../../communication/include/pdu.h
UpperBodyClass$(GL_EXT)$(BIN_EXT).o: ../../communication/include/envirvalues.h
