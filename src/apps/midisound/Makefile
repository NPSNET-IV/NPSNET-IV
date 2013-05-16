# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre
include ../../Makefile.exe_pre

# You can change these flags while testing for optimizations
OPT	=	-O2
#CFLAGS	=	$(OPT)
#OBJLIST	=

# You can define include directories your code needs here --
#    other than ../include
APPINCDIRS	=	-I../../communication/include \
			-I../../media/include \
			-I../../setup/include

# You can define your own standard include files, if necessary
STDINCS =	../../communication/include/disnetlib.h \
	../../communication/include/disconvert.h \
	../../communication/include/local_snd_code.h \
	../../communication/include/midi_snd_code.h \
	../../media/include/sound.h \
	../../setup/include/fileio.h \
	../../setup/include/setup_const.h \
	../../setup/include/im.tab.h \
	../../setup/include/imstructs.h \
	../../setup/include/imclass.h \
	../../setup/include/imdefaults.h \
	constants.h globals.h typedefs.h


# You can define include directories your code needs here --
APPLIBDIRS	=	-L$(BINDIR)lib -rpath ./bin/lib


# You can include your own library flags (i.e. -llib) here --
APPLIBFLAGS	=	-ldisnet$(BIN_EXT) -ldisconvert$(BIN_EXT) \
			-lLoral_round$(BIN_EXT) -lidentify$(BIN_EXT) \
			-linitman$(BIN_EXT) -lmpc -lfm -lgl -lm -ly -ll 

LIBDEP  =       $(BINDIR)lib/libdisnet$(BIN_EXT).$(LIB_EXT) \
		$(BINDIR)lib/libdisconvert$(BIN_EXT).$(LIB_EXT) \
                $(BINDIR)lib/libLoral_round$(BIN_EXT).$(LIB_EXT) \
		$(BINDIR)lib/libidentify$(BIN_EXT).$(LIB_EXT) \
		$(BINDIR)lib/libinitman$(BIN_EXT).$(LIB_EXT)

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

TARGETS =       $(BINDIR)npsmidi$(BIN_EXT)
OBJECTS	=	dr$(BIN_EXT).o get_config$(BIN_EXT).o soundlib$(BIN_EXT).o sound_main$(BIN_EXT).o screen$(BIN_EXT).o


# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.exe_post


# ***********************************************************************
# ***** ALWAYS list object file dependencies here

dr$(BIN_EXT).o:		dr.cc dr.h $(STDINCS)
get_config$(BIN_EXT).o:	get_config.cc get_config.h $(STDINCS)
soundlib$(BIN_EXT).o:	soundlib.cc soundlib.h $(STDINCS)
sound_main$(BIN_EXT).o:	sound_main.cc dr.h screen.h $(STDINCS) $(LIBDEP)
screen$(BIN_EXT).o:	screen.cc screen.h $(STDINCS)

# ***********************************************************************
