
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
	@ $(MAKE) -e -f Makefile.bird ${OPTION}
	@ $(MAKE) -e -f Makefile.fastrak ${OPTION}
	@ $(MAKE) -e -f Makefile.fcs ${OPTION}
	@ $(MAKE) -e -f Makefile.fcs_debug ${OPTION}
	@ $(MAKE) -e -f Makefile.flybox ${OPTION}
	@ $(MAKE) -e -f Makefile.leather_flybox ${OPTION}
	@ $(MAKE) -e -f Makefile.isotrack ${OPTION}
	@ $(MAKE) -e -f Makefile.fingerport ${OPTION}
	@ $(MAKE) -e -f Makefile.odt_test ${OPTION}
#	@ $(MAKE) -e -f Makefile.viminit ${OPTION}
