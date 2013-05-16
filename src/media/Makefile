
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
	@ echo "************ MEDIA LIBRARIES  ***********"
	@ echo "Audio library: Making ${OPTION}"
	cd audio;$(MAKE) -e ${OPTION}
	@ echo "Video library: Making ${OPTION}"
	cd video;$(MAKE) -e ${OPTION}

npscode:
	@ echo "+++++++++++   NPSNET MEDIA   ++++++++++++"
	cd npsnet;$(MAKE) -e ${OPTION}
