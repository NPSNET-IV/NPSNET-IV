
SHELL	=	/bin/sh

all:
	@ $(MAKE) -f Makefile.disconvert
	@ $(MAKE) -f Makefile.disnet
	@ $(MAKE) -f Makefile.disbridge
	@ $(MAKE) -f Makefile.identify
	@ $(MAKE) -f Makefile.nps_converter

clobber:
	@ $(MAKE) -f Makefile.disconvert clobber
	@ $(MAKE) -f Makefile.disnet clobber
	@ $(MAKE) -f Makefile.disbridge clobber
	@ $(MAKE) -f Makefile.identify clobber
	@ $(MAKE) -f Makefile.nps_converter clobber

clean:
	@ $(MAKE) -f Makefile.disconvert clean
	@ $(MAKE) -f Makefile.disnet clean
	@ $(MAKE) -f Makefile.disbridge clean
	@ $(MAKE) -f Makefile.identify clean
	@ $(MAKE) -f Makefile.nps_converter clean

depend:
	@ $(MAKE) -f Makefile.disconvert depend
	@ $(MAKE) -f Makefile.disnet depend
	@ $(MAKE) -f Makefile.disbridge depend
	@ $(MAKE) -f Makefile.identify depend
	@ $(MAKE) -f Makefile.nps_converter depend

