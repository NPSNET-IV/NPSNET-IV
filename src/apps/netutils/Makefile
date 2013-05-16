
SHELL	=	/bin/sh

all:
	@OPTION=all;export OPTION; \
	$(MAKE) recursive

clean:
	@OPTION=clean;export OPTION; \
	$(MAKE) recursive

clobber:
	@OPTION=clobber;export OPTION; \
	$(MAKE) recursive

recursive:
	@ $(MAKE) -e -f Makefile.sockcommtest ${OPTION}
	@ $(MAKE) -e -f Makefile.nps_to_lw ${OPTION}
	@ $(MAKE) -e -f Makefile.lw_to_nps_read ${OPTION}
	@ $(MAKE) -e -f Makefile.lw_to_nps_write ${OPTION}
