SHELL=/bin/sh

all:
	@ ./sound.test
	@ $(MAKE) -e -f Makefile.npsmono

clean:
	@ ./sound.test
	@ $(MAKE) -e -f Makefile.npsmono clean
	@ set nonomatch; rm -f Makefile.config

clobber:
	@ ./sound.test
	@ $(MAKE) -e -f Makefile.npsmono clobber
	@ set nonomatch; rm -f Makefile.config

