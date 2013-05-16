# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
#OPT	=	-O2
#CFLAGS	=	$(OPT)

# You can locally define any symbols you need using the next line
#DEFINES	=

# You can define include directories your code needs here --
#    other than ../include
APPINCDIRS	= \
	-I/usr/include/Performer -I/usr/src/Performer/include -I/usr/include

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	bbox.cc models.cc
TARGETS =       $(LIBDIR)libmodels$(GL_EXT)$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	bbox$(GL_EXT)$(BIN_EXT).o \
		models$(GL_EXT)$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

bbox$(GL_EXT)$(BIN_EXT).o: /usr/include/bstring.h ../include/bbox.h /usr/include/Performer/pf.h
bbox$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/pr.h /usr/include/sys/types.h
bbox$(GL_EXT)$(BIN_EXT).o: /usr/include/standards.h /usr/include/sgidefs.h /usr/include/stdio.h
bbox$(GL_EXT)$(BIN_EXT).o: /usr/include/ulocks.h /usr/include/limits.h /usr/include/task.h
bbox$(GL_EXT)$(BIN_EXT).o: /usr/include/sys/prctl.h /usr/include/unistd.h /usr/include/errno.h
bbox$(GL_EXT)$(BIN_EXT).o: /usr/include/sys/errno.h /usr/include/X11/X.h /usr/include/X11/Xlib.h
bbox$(GL_EXT)$(BIN_EXT).o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
bbox$(GL_EXT)$(BIN_EXT).o: /usr/include/stddef.h /usr/include/X11/Xutil.h /usr/include/GL/gl.h
bbox$(GL_EXT)$(BIN_EXT).o: /usr/include/GL/glx.h /usr/include/X11/Xmd.h
bbox$(GL_EXT)$(BIN_EXT).o: /usr/include/GL/glxtokens.h /usr/include/GL/glu.h
bbox$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/opengl.h /usr/include/Performer/prmath.h
bbox$(GL_EXT)$(BIN_EXT).o: /usr/include/Performer/prstats.h /usr/include/Performer/pfstats.h
