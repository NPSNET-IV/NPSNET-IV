
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
	@ echo "********* DATABASE LIBRARIES ************"
	@ echo "Loader library: Making ${OPTION}"
	cd loaders;$(MAKE) -e ${OPTION}
	@ echo "Models library: Making ${OPTION}"
	cd models;$(MAKE) -e ${OPTION}
	@ echo "Terrain library: Making ${OPTION}"
	cd terrain;$(MAKE) -e ${OPTION}
	@ echo "Paging library: Making ${OPTION}"
	cd db_paging;$(MAKE) -e ${OPTION}
	@ echo "TSG library: Making ${OPTION}"
	cd tsg;$(MAKE) -e ${OPTION}

npscode:
	@ echo "++++++++++   NPSNET DATABASE   ++++++++++"
	@echo "All files have been moved out the the npsnet directory"
