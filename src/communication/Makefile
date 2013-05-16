
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
	@ echo "****** COMMUNICATION LIBRARIES **********"
	@ echo "DIS network library: Making ${OPTION}"
	cd dis;$(MAKE) -e ${OPTION}
	@ echo "Socket communications network library: Making ${OPTION}"
	cd sockcomm;$(MAKE) -e ${OPTION}
	@ echo "Interface network library: Making ${OPTION}"
	cd interface;$(MAKE) -e ${OPTION}
	@ echo "Socket library: Making ${OPTION}"
	cd socket;$(MAKE) -e ${OPTION}
	@ echo "TCP/IP library: Making ${OPTION}"
	cd tcpip;$(MAKE) -e ${OPTION}
	@ echo "ModSAF PO network library: Making ${OPTION}"
	cd modsafpo;$(MAKE) -e ${OPTION}

npscode:
	@ echo "++++++++ NPSNET COMMUNICATION +++++++++++"
	cd npsnet;$(MAKE) -e ${OPTION}
