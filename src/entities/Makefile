
.SILENT:

SHELL	=	/bin/sh

default:	all

all:
	@ OPTION=all; export OPTION; \
	$(MAKE) -e librecursive; \
	$(MAKE) -e npscode

libs:
	@ OPTION=all; export OPTION; \
	$(MAKE) -e librecursive

npsnet:	iscool

iscool:
	@ OPTION=all;export OPTION; \
	$(MAKE) -e npscode


install:
	@ OPTION=install;export OPTION; \
	$(MAKE) -e librecursive; \
	$(MAKE) -e npscode

clean:
	@ OPTION=clean; export OPTION; \
	$(MAKE) -e librecursive; \
	$(MAKE) -e npscode

clobber:
	@ OPTION=clobber; export OPTION; \
	$(MAKE) -e librecursive; \
	$(MAKE) -e npscode

depend:
	@ OPTION=depend; export OPTION; \
	$(MAKE) -e librecursive; \
	$(MAKE) -e npscode

librecursive:
	@ echo "**********  ENTITY LIRBRARIES  **********"
	@ echo "Air entities library: Making ${OPTION}"
	cd air;$(MAKE) -e ${OPTION}
	@ echo "Dead Reckoning library: Making ${OPTION}"
	cd deadreckoning;$(MAKE) -e ${OPTION}
	@ echo "Entity library: Making ${OPTION}"
	cd entity;$(MAKE) -e ${OPTION}
	@ echo "Ground library: Making ${OPTION}"
	cd ground;$(MAKE) -e ${OPTION}
	@ echo "Human entities library: Making ${OPTION}"
	cd human;$(MAKE) -e ${OPTION}
	@ echo "Jack entities library: Making ${OPTION}"
	cd jack;$(MAKE) -e ${OPTION}
	@ echo "Lifeform entities library: Making ${OPTION}"
	cd lifeform;$(MAKE) -e ${OPTION}
	@ echo "Munition entities library: Making ${OPTION}"
	cd munition;$(MAKE) -e ${OPTION}
	@ echo "Space library: Making ${OPTION}"
	cd space;$(MAKE) -e ${OPTION}
	@ echo "Stealth library: Making ${OPTION}"
	cd stealth;$(MAKE) -e ${OPTION}
	@ echo "Subsurface entities library: Making ${OPTION}"
	cd subsurface;$(MAKE) -e ${OPTION}
	@ echo "Surface entities library: Making ${OPTION}"
	cd surface;$(MAKE) -e ${OPTION}
	@ echo "DI_Guy entities library: Making ${OPTION}"
	cd diguy;$(MAKE) -e ${OPTION}
	@ echo "Vehicle entities library: Making ${OPTION}"
	cd vehicle;$(MAKE) -e ${OPTION}
#	@ echo "Dude entities library: Making ${OPTION}"
#	cd dude;$(MAKE) -e ${OPTION}

npscode:
	@ echo "++++++++++   NPSNET ENTITIES   ++++++++++"
	cd npsnet;$(MAKE) -e ${OPTION}
