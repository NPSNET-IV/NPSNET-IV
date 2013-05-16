
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
	@ echo "********* ENVIRONMENT LIBRAIRES *********"
	@ echo "Cloud library: Making ${OPTION}"
	cd clouds;$(MAKE) -e ${OPTION}
	@ echo "Lighting library: Making ${OPTION}"
	cd lighting;$(MAKE) -e ${OPTION}
	@ echo "Smoke library: Making ${OPTION}"
	cd smoke;$(MAKE) -e ${OPTION}
	@ echo "Weather library: Making ${OPTION}"
	cd weather;$(MAKE) -e ${OPTION}

npscode:
	@ echo "++++++++++ NPSNET ENVIRONMENT  ++++++++++"
	cd npsnet;$(MAKE) -e ${OPTION}
