
SHELL	=	/bin/sh

all:
	@OPTION=all;export OPTION; \
	$(MAKE) recursive

depend:
	@OPTION=depend;export OPTION; \
	$(MAKE) recursive

clean:
	@OPTION=clean;export OPTION; \
	$(MAKE) recursive

clobber:
	@OPTION=clobber;export OPTION; \
	$(MAKE) recursive

recursive:
	@ $(MAKE) -e -f Makefile.subcontrols ${OPTION}
	
