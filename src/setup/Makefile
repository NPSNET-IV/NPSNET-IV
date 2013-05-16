
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
	@ echo "************ SETUP LIBRARIES  ***********"
	@ echo "Command line library: Making ${OPTION}"
	cd commandline;$(MAKE) -e ${OPTION}
	@ echo "Configuration library: Making ${OPTION}"
	cd config;$(MAKE) -e ${OPTION}
	@ echo "C++ File Input/Output library: Making ${OPTION}"
	cd fileio;$(MAKE) -e ${OPTION}
	@ echo "Initialization Manager library: Making ${OPTION}"
	cd initman;$(MAKE) -e ${OPTION}

npscode:
	@ echo "+++++++++++   NPSNET SETUP   ++++++++++++"
	cd npsnet;$(MAKE) -e ${OPTION}
