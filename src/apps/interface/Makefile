
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
	@ $(MAKE) -e -f Makefile.idusend ${OPTION}
	@ $(MAKE) -e -f Makefile.idujack ${OPTION}
	@ $(MAKE) -e -f Makefile.idurecv ${OPTION}
	@ $(MAKE) -e -f Makefile.fileio ${OPTION}
	@ $(MAKE) -e -f Makefile.npsfscanf ${OPTION}

