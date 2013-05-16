
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
	@ $(MAKE) -e -f Makefile.disbridge ${OPTION}
	@ $(MAKE) -e -f Makefile.pdudisplay ${OPTION}
	@ $(MAKE) -e -f Makefile.pdudump ${OPTION}
	@ $(MAKE) -e -f Makefile.populate ${OPTION}
	@ $(MAKE) -e -f Makefile.identify_dis ${OPTION}
	@ $(MAKE) -e -f Makefile.spew ${OPTION}

