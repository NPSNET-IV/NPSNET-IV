SHELL=/bin/sh

all:
	@ ./sound.test
	@ $(MAKE) -e -f Makefile.sound

clean:
	@ ./sound.test
	@ $(MAKE) -e -f Makefile.sound clean
	@ set nonomatch; rm -f Makefile.config

clobber:
	@ ./sound.test
	@ $(MAKE) -e -f Makefile.sound clobber
	@ set nonomatch; rm -f Makefile.config

depend:
	@ ./sound.test
	@ $(MAKE) -e -f Makefile.sound depend
