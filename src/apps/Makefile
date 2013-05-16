
.SILENT:

SHELL	=	/bin/sh

default:	all

all:
	@ OPTION=all; export OPTION; \
	$(MAKE) -e recursive;

libs:


install:
	@ OPTION=install;export OPTION; \
	$(MAKE) -e recursive

clean:
	@ OPTION=clean; export OPTION; \
	$(MAKE) -e recursive

clobber:
	@ OPTION=clobber; export OPTION; \
	$(MAKE) -e recursive

depend:

distools:	distoolsapp

distoolsapp:
	@ echo "DIS Tools: Making all"
	cd distools;$(MAKE) -e all

inpututils:	inpututilsapp

inpututilsapp:
	@ echo "Input Utilities: Making all"
	cd inpututils;$(MAKE) -e all

interface:	interfaceapp

interfaceapp:
	@ echo "Interface: Making all"
	cd interface;$(MAKE) -e all

midisound:	midisoundapp

midisoundapp:
	@ echo "MIDI Sound Server: Making all"
	cd midisound;$(MAKE) -e all

monosound:	monosoundapp

monosoundapp:
	@ echo "MONO Sound Server: Making all"
	cd monosound;$(MAKE) -e all

npsnet:	npsnetapp

npsnetapp:
	@ echo "NPSNET: Will build during make all phase."

panel:	panelapp

panelapp:
	@ echo "Panel: Making all"
	cd panel;$(MAKE) -e all

shippanel:	shippanelapp

shippanelapp:
	@ echo "Ship Panel: Making all"
	cd shippanel;$(MAKE) -e all

recursive:
	@ echo "************* APPLICATIONS **************"
	@ echo "DIS Tools: Making ${OPTION}"
	cd distools;$(MAKE) -e ${OPTION}
	@ echo "Network utilities: Making ${OPTION}"
	cd netutils;$(MAKE) -e ${OPTION}
	@ echo "Input Utilities: Making ${OPTION}"
	cd inpututils;$(MAKE) -e ${OPTION}
	@ echo "Interface: Making ${OPTION}"
	cd interface;$(MAKE) -e ${OPTION}
	@ echo "MIDI Sound Server: Making ${OPTION}"
	cd midisound;$(MAKE) -e ${OPTION}
	@ echo "MONO Sound Server: Making ${OPTION}"
	cd monosound;$(MAKE) -e ${OPTION}
	@ echo "NPSNET: Making ${OPTION}"
	cd npsnet;$(MAKE) -e ${OPTION}
#	@ echo "Panel: Making ${OPTION}"
#	cd panel;$(MAKE) -e ${OPTION}
#	@ echo "Ship Panel: Making ${OPTION}"
#	cd shippanel;$(MAKE) -e ${OPTION}
