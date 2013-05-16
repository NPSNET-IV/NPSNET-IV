
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
	@ echo "**********  DISPLAY LIBRARIES  **********"
	@ echo "Curses display library: Making ${OPTION}"
	cd curses;$(MAKE) -e ${OPTION}
	@ echo "Fonts display library: Making ${OPTION}"
	cd fonts;$(MAKE) -e ${OPTION}
	@ echo "GL display library: Making ${OPTION}"
	cd gl;$(MAKE) -e ${OPTION}
	@ echo "Performer display library: Making ${OPTION}"
	cd performer;$(MAKE) -e ${OPTION}
	@ echo "X display library: Making ${OPTION}"
	cd x;$(MAKE) -e ${OPTION}

npscode:
	@ echo "++++++++++   NPSNET DISPLAY    ++++++++++"
	cd npsnet;$(MAKE) -e ${OPTION}
