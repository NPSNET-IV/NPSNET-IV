# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE        =       Makefile

# You can change these flags while testing for optimizations
OPT     =       -g
#CFLAGS =       $(OPT)

# You can locally define any symbols you need using the next line
#DEFINES =

# You can define include directories your code needs here --
#    other than ../include
APPINCDIRS      =       -I../../apps/include -I../../c2/include \
	-I../../communication/include -I../../database/include \
	-I../../display/include -I../../entities/include \
	-I../../environment/include -I../../interface/include \
	-I../../media/include -I../../setup/include -I../../utilities/include \
	-I/usr/include/Performer -I/usr/src/Performer/include \
	-I../../../otherlibs/Jack/JackML
	

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES =       dude.cc dude_funcs.cc filter.cc jade.cc
TARGETS =       $(LIBDIR)libdude.a
OBJECTS =       dude.o dude_funcs.o filter.o jade.o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

dude.o: ../include/dude.h ../include/person.h ../include/vehicle.h
dude.o: ../include/entity_const.h ../include/weapon.h
dude.o: /usr/include/Performer/pf.h ../../communication/include/pdu.h
dude.o: ../../communication/include/identify.h ../../display/include/draw.h
dude.o: ../../apps/include/common_defs.h ../../setup/include/setup_const.h
dude.o: ../../display/include/display_const.h ../include/human_const.h
dude.o: ../include/filter.h ../../apps/include/common_defs2.h
dude.o: ../../apps/include/constants.h
dude.o: ../../communication/include/appearance.h
dude.o: ../../communication/include/disnetlib.h
dude.o: ../../communication/include/netutil.h
dude.o: ../../database/include/terrain.h
dude.o: ../../utilities/include/conversion_const.h
dude.o: ../include/collision_const.h
dude_funcs.o: ../include/dude_funcs.h ../include/dude.h ../include/person.h
dude_funcs.o: ../include/vehicle.h ../include/entity_const.h
dude_funcs.o: ../include/weapon.h /usr/include/Performer/pf.h
dude_funcs.o: ../../communication/include/pdu.h
dude_funcs.o: ../../communication/include/identify.h
dude_funcs.o: ../../display/include/draw.h ../../apps/include/common_defs.h
dude_funcs.o: ../../setup/include/setup_const.h
dude_funcs.o: ../../display/include/display_const.h ../include/human_const.h
dude_funcs.o: /usr/include/Performer/pfflt.h
filter.o: ../include/filter.h ../../apps/include/common_defs2.h
filter.o: /usr/include/Performer/pf.h ../../apps/include/constants.h
filter.o: ../../communication/include/pdu.h ../../apps/include/common_defs.h
filter.o: ../../setup/include/setup_const.h
filter.o: ../../display/include/display_const.h ../include/vehicle.h
filter.o: ../include/entity_const.h ../include/weapon.h
filter.o: ../../communication/include/identify.h ../../display/include/draw.h
jade.o: ../include/jade.h ../include/person.h ../include/vehicle.h
jade.o: ../include/entity_const.h ../include/weapon.h
jade.o: /usr/include/Performer/pf.h ../../communication/include/pdu.h
jade.o: ../../communication/include/identify.h ../../display/include/draw.h
jade.o: ../../apps/include/common_defs.h ../../setup/include/setup_const.h
jade.o: ../../display/include/display_const.h ../include/human_const.h
jade.o: ../include/dude.h ../include/jack_di_veh.h
jade.o: ../../../otherlibs/Jack/JackML/jacksoldier.h
jade.o: ../../../otherlibs/Jack/JackML/jackPacket.h ../include/jointmods.h
jade.o: ../include/formation.h ../include/signals.h
jade.o: ../../apps/include/common_defs2.h ../../apps/include/constants.h
jade.o: ../../communication/include/appearance.h
jade.o: ../../communication/include/disnetlib.h
jade.o: ../../communication/include/netutil.h
jade.o: ../../database/include/terrain.h
jade.o: ../../utilities/include/conversion_const.h
jade.o: ../include/collision_const.h ../../../otherlibs/Jack/JackML/jack.h
