
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
	@ echo "********** INTERFACE LIBRARIES **********"
	@ echo "Advanced Hooking Algorithm library: Making ${OPTION}"
	cd aha;$(MAKE) -e ${OPTION}
	@ echo "Ascension Bird library: Making ${OPTION}"
	cd bird;$(MAKE) -e ${OPTION}
	@ echo "Polhemus Fastrak device driver library: Making ${OPTION}"
	cd fastrak;$(MAKE) -e ${OPTION}
	@ echo "HMD tracking library: Making ${OPTION}"
	cd hmd;$(MAKE) -e ${OPTION}
	@ echo "Thrust Master FCS library: Making ${OPTION}"
	cd fcs;$(MAKE) -e ${OPTION}
	@ echo "BG Systems Flybox library: Making ${OPTION}"
	cd flybox;$(MAKE) -e ${OPTION}
	@ echo "Leather Net Flybox library: Making ${OPTION}"
	cd leather_flybox;$(MAKE) -e ${OPTION}
	@ echo "Kinney Aero Flight Controls library: Making ${OPTION}"
	cd kaflight;$(MAKE) -e ${OPTION}
	@ echo "Keyboard & Mouse library: Making ${OPTION}"
	cd keyboard;$(MAKE) -e ${OPTION}
	@ echo "I-Port library: Making ${OPTION}"
	cd iport;$(MAKE) -e ${OPTION}
	@ echo "Upper Body library: Making ${OPTION}"
	cd upperbody;$(MAKE) -e ${OPTION}
	@ echo "Input Manager: Making ${OPTION}"
	cd manager;$(MAKE) -e ${OPTION}
	@ echo "Panel: Making ${OPTION}"
	cd panel;$(MAKE) -e ${OPTION}
	@ echo "Fakespace: Making ${OPTION}"
	cd fakespace;$(MAKE) -e ${OPTION}
	@ echo "lw_tcpip: Making ${OPTION}"
	cd lw_tcpip;$(MAKE) -e ${OPTION}
	@ echo "ODT: Making ${OPTION}"
	cd odt;$(MAKE) -e ${OPTION}

npscode:
	@ echo "++++++++++   NPSNET INTERFACE  ++++++++++"
	cd npsnet;$(MAKE) -e ${OPTION}
