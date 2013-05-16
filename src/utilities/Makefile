
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
	@ echo "********** UTILITIES LIBRARIES **********"
	@ echo "Conversions library: Making ${OPTION}"
	cd conversions;$(MAKE) -e ${OPTION}
	@ echo "Error Handling library: Making ${OPTION}"
	cd errorhandling;$(MAKE) -e ${OPTION}
	@ echo "General utilities library: Making ${OPTION}"
	cd general;$(MAKE) -e ${OPTION}
	@ echo "Multi-processing library: Making ${OPTION}"
	cd multiprocessing;$(MAKE) -e ${OPTION}
	@ echo "Quaternions library: Making ${OPTION}"
	cd quaternions;$(MAKE) -e ${OPTION}

npscode:
	@ echo "++++++++++   NPSNET UTILITIES  ++++++++++"
	cd npsnet;$(MAKE) -e ${OPTION}
