
.SILENT:

SHELL	=	/bin/sh

default:	all

all:
	@ OPTION=libs; export OPTION; \
	MFLAGS=e; export MFLAGS; \
	$(MAKE) -e recursive
	@ OPTION=npsnet; export OPTION; \
	MFLAGS=e; export MFLAGS; \
	$(MAKE) -e recursive
	@ cd apps;$(MAKE) -e all

switch_igl:
	@ cp GLtype_igl GLtype

switch_ogl:
	@ cp GLtype_ogl GLtype

switch_o32:
	@ cp Libtype_o32 Libtype

switch_n32:
	@ cp Libtype_n32 Libtype

both: igl ogl

every:
	@ cp Libtype_o32 Libtype
	@ OPTION=libs; export OPTION; \
	MFLAGS=e; export MFLAGS; \
	$(MAKE) -e igl
	@ OPTION=npsnet; export OPTION; \
	MFLAGS=e; export MFLAGS; \
	$(MAKE) -e ogl
	@ cp Libtype_n32 Libtype
	@ OPTION=libs; export OPTION; \
	MFLAGS=e; export MFLAGS; \
	$(MAKE) -e igl
	@ OPTION=npsnet; export OPTION; \
	MFLAGS=e; export MFLAGS; \
	$(MAKE) -e ogl

igl:
	@ cp GLtype_igl GLtype
	@ OPTION=libs; export OPTION; \
	MFLAGS=e; export MFLAGS; \
	$(MAKE) -e recursive
	@ OPTION=npsnet; export OPTION; \
	MFLAGS=e; export MFLAGS; \
	$(MAKE) -e recursive
	@ cd apps;$(MAKE) -e all

ogl:
	@ cp GLtype_ogl GLtype
	@ OPTION=libs; export OPTION; \
	MFLAGS=e; export MFLAGS; \
	$(MAKE) -e recursive
	@ OPTION=npsnet; export OPTION; \
	$(MAKE) -e recursive
	@ cd apps;$(MAKE) -e all

force:
	@ OPTION=libs; export OPTION; \
	MFLAGS=eu; export MFLAGS; \
	$(MAKE) -eu recursive
	@ OPTION=npsnet; export OPTION; \
	MFLAGS=eu; export MFLAGS; \
	$(MAKE) -eu recursive
	@ cd apps;$(MAKE) -eu all
	
libs:
	@ OPTION=libs; export OPTION; \
	$(MAKE) -e recursive

apps:	makeapps

makeapps:
	@ cd apps;$(MAKE) -e all

install:
	@ OPTION=depend;export OPTION; \
	$(MAKE) -e depend
	@ OPTION=config;export OPTION; \
	$(MAKE) -e config

config:
	@ ../bin/config_make
	@ OPTION=libs; export OPTION; \
	MFLAGS=e; export MFLAGS; \
	$(MAKE) -e recursive
	@ OPTION=npsnet; export OPTION; \
	$(MAKE) -e recursive
	@ cd apps;$(MAKE) -e all

clean:
	@ OPTION=clean; export OPTION; \
	$(MAKE) -e recursive

clean_both:
	@ cp GLtype_igl GLtype
	@ OPTION=clean; export OPTION; \
	$(MAKE) -e recursive
	@ cp GLtype_ogl GLtype
	@ OPTION=clean; export OPTION; \
	$(MAKE) -e recursive

clean_igl:
	@ cp GLtype_igl GLtype
	@ OPTION=clean; export OPTION; \
	$(MAKE) -e recursive

clean_ogl:
	@ cp GLtype_ogl GLtype
	@ OPTION=clean; export OPTION; \
	$(MAKE) -e recursive

clobber:
	@ OPTION=clobber; export OPTION; \
	$(MAKE) -e recursive

clobber_both:
	@ cp GLtype_igl GLtype
	@ OPTION=clobber; export OPTION; \
	$(MAKE) -e recursive
	@ cp GLtype_ogl GLtype
	@ OPTION=clobber; export OPTION; \
	$(MAKE) -e recursive

clobber_igl:
	@ cp GLtype_igl GLtype
	@ OPTION=clobber; export OPTION; \
	$(MAKE) -e recursive

clobber_ogl:
	@ cp GLtype_ogl GLtype
	@ OPTION=clobber; export OPTION; \
	$(MAKE) -e recursive

depend:
	@ OPTION=depend; export OPTION; \
	$(MAKE) -e recursive

recursive:
	@ echo "********** TOP-LEVEL MAKEFILE  **********"
	@ echo "Setup: Making ${OPTION}"
	cd setup;$(MAKE) -${MFLAGS} ${OPTION}
	@ echo "C2: Making ${OPTION}"
	cd c2;$(MAKE) -${MFLAGS} ${OPTION}
	@ echo "Communication: Making ${OPTION}"
	cd communication;$(MAKE) -${MFLAGS} ${OPTION}
	@ echo "Database: Making ${OPTION}"
	cd database;$(MAKE) -${MFLAGS} ${OPTION}
	@ echo "Display: Making ${OPTION}"
	cd display;$(MAKE) -${MFLAGS} ${OPTION}
	@ echo "Entities: Making ${OPTION}"
	cd entities;$(MAKE) -${MFLAGS} ${OPTION}
	@ echo "Environment: Making ${OPTION}"
	cd environment;$(MAKE) -${MFLAGS} ${OPTION}
	@ echo "Interface: Making ${OPTION}"
	cd interface;$(MAKE) -${MFLAGS} ${OPTION}
	@ echo "Media: Making ${OPTION}"
	cd media;$(MAKE) -${MFLAGS} ${OPTION}
	@ echo "Utilities: Making ${OPTION}"
	cd utilities;$(MAKE) -${MFLAGS} ${OPTION}
	@ echo "Apps: Making ${OPTION}"
	cd apps;$(MAKE) -${MFLAGS} ${OPTION}

