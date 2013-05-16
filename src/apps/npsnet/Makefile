SHELL=/bin/sh

all:
	@ ./sound.test
	@ $(MAKE) -e -f Makefile.npsnet

clean:
	@ ./sound.test
	@ $(MAKE) -e -f Makefile.npsnet clean
	@ set nonomatch; rm -f Makefile.config

clobber:
	@ ./sound.test
	@ $(MAKE) -e -f Makefile.npsnet clobber
	@ set nonomatch; rm -f Makefile.config

