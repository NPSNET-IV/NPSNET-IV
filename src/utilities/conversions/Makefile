# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
OPT	=	-O2 -ansi
#OPT	=	-g
#CFLAGS	=	$(OPT)

# You can locally define any symbols you need using the next line
#DEFINES	=

# You can define include directories your code needs here --
#    other than ../include
APPINCDIRS	= -I/usr/include

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	coord_convert.c coord_grid.c coord_tcc.c \
	coord_format.c coord_init.c coord_trans.c rdr_symbol.c \
	rdr_tags.c rdr_lexer.c rdr_parser.c vmat_angle.c \
	vmat_add.c vmat_cross.c vmat_dot.c vmat_matmat.c \
	vmat_negate.c vmat_project.c vmat_rotate.c vmat_scal.c \
	vmat_trans.c vmat_unit.c vmat_vecmat.c
TARGETS =       $(LIBDIR)libLoral_round$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	coord_convert$(BIN_EXT).o \
		coord_grid$(BIN_EXT).o \
		coord_tcc$(BIN_EXT).o \
		coord_format$(BIN_EXT).o \
		coord_init$(BIN_EXT).o \
		coord_trans$(BIN_EXT).o \
		rdr_symbol$(BIN_EXT).o \
		rdr_tags$(BIN_EXT).o \
		rdr_lexer$(BIN_EXT).o \
		rdr_parser$(BIN_EXT).o \
		vmat_angle$(BIN_EXT).o \
		vmat_add$(BIN_EXT).o \
		vmat_cross$(BIN_EXT).o \
		vmat_dot$(BIN_EXT).o \
		vmat_matmat$(BIN_EXT).o \
		vmat_negate$(BIN_EXT).o \
		vmat_project$(BIN_EXT).o \
		vmat_rotate$(BIN_EXT).o \
		vmat_scal$(BIN_EXT).o \
		vmat_trans$(BIN_EXT).o \
		vmat_unit$(BIN_EXT).o \
		vmat_vecmat$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

coord_convert$(BIN_EXT).o: ../include/libcoord_local.h ../include/libcoordinates.h
coord_convert$(BIN_EXT).o: ../include/stdtypes.h /usr/include/ctype.h
coord_convert$(BIN_EXT).o: /usr/include/standards.h /usr/include/varargs.h
coord_convert$(BIN_EXT).o: /usr/include/sgidefs.h /usr/include/strings.h
coord_convert$(BIN_EXT).o: /usr/include/sys/types.h
coord_grid$(BIN_EXT).o: ../include/libcoord_local.h ../include/libcoordinates.h
coord_grid$(BIN_EXT).o: ../include/stdtypes.h ../include/stdext.h /usr/include/math.h
coord_grid$(BIN_EXT).o: /usr/include/sgidefs.h /usr/include/standards.h
coord_tcc$(BIN_EXT).o: ../include/libcoord_local.h ../include/libcoordinates.h
coord_tcc$(BIN_EXT).o: ../include/stdtypes.h /usr/include/math.h /usr/include/sgidefs.h
coord_tcc$(BIN_EXT).o: /usr/include/standards.h ../include/stdext.h
coord_tcc$(BIN_EXT).o: ../include/stdalloc.h /usr/include/malloc.h
coord_format$(BIN_EXT).o: /usr/include/ctype.h /usr/include/standards.h
coord_format$(BIN_EXT).o: ../include/libcoord_local.h ../include/libcoordinates.h
coord_format$(BIN_EXT).o: ../include/stdtypes.h
coord_init$(BIN_EXT).o: /usr/include/ctype.h /usr/include/standards.h
coord_init$(BIN_EXT).o: ../include/libcoord_local.h ../include/libcoordinates.h
coord_init$(BIN_EXT).o: ../include/stdtypes.h ../include/libreader.h
coord_trans$(BIN_EXT).o: ../include/libcoord_local.h ../include/libcoordinates.h
coord_trans$(BIN_EXT).o: ../include/stdtypes.h ../include/libvecmat.h
coord_trans$(BIN_EXT).o: ../include/stdext.h /usr/include/math.h /usr/include/sgidefs.h
coord_trans$(BIN_EXT).o: /usr/include/standards.h
rdr_symbol$(BIN_EXT).o: /usr/include/strings.h /usr/include/standards.h
rdr_symbol$(BIN_EXT).o: /usr/include/sys/types.h /usr/include/sgidefs.h
rdr_symbol$(BIN_EXT).o: ../include/librdr_local.h ../include/libreader.h
rdr_symbol$(BIN_EXT).o: ../include/stdtypes.h ../include/stdalloc.h
rdr_symbol$(BIN_EXT).o: /usr/include/malloc.h
rdr_tags$(BIN_EXT).o: ../include/librdr_local.h ../include/libreader.h
rdr_tags$(BIN_EXT).o: ../include/stdtypes.h /usr/include/stdio.h
rdr_tags$(BIN_EXT).o: /usr/include/standards.h /usr/include/sgidefs.h
rdr_tags$(BIN_EXT).o: /usr/include/varargs.h /usr/include/stdlib.h
rdr_lexer$(BIN_EXT).o: /usr/include/stdio.h /usr/include/standards.h
rdr_lexer$(BIN_EXT).o: /usr/include/sgidefs.h ../include/librdr_local.h
rdr_lexer$(BIN_EXT).o: ../include/libreader.h ../include/stdtypes.h ../include/y.tab.h
rdr_parser$(BIN_EXT).o: ../include/librdr_local.h ../include/libreader.h
rdr_parser$(BIN_EXT).o: ../include/stdtypes.h /usr/include/stdio.h
rdr_parser$(BIN_EXT).o: /usr/include/standards.h /usr/include/sgidefs.h
rdr_parser$(BIN_EXT).o: ../include/stdalloc.h /usr/include/malloc.h
rdr_parser$(BIN_EXT).o: /usr/include/strings.h /usr/include/sys/types.h
vmat_angle$(BIN_EXT).o: ../include/libvecmat.h ../include/stdtypes.h
vmat_angle$(BIN_EXT).o: ../include/stdext.h /usr/include/math.h /usr/include/sgidefs.h
vmat_angle$(BIN_EXT).o: /usr/include/standards.h
vmat_add$(BIN_EXT).o: ../include/libvecmat.h ../include/stdtypes.h ../include/stdext.h
vmat_add$(BIN_EXT).o: /usr/include/math.h /usr/include/sgidefs.h
vmat_add$(BIN_EXT).o: /usr/include/standards.h
vmat_cross$(BIN_EXT).o: ../include/libvecmat.h ../include/stdtypes.h
vmat_cross$(BIN_EXT).o: ../include/stdext.h /usr/include/math.h /usr/include/sgidefs.h
vmat_cross$(BIN_EXT).o: /usr/include/standards.h
vmat_dot$(BIN_EXT).o: ../include/libvecmat.h ../include/stdtypes.h ../include/stdext.h
vmat_dot$(BIN_EXT).o: /usr/include/math.h /usr/include/sgidefs.h
vmat_dot$(BIN_EXT).o: /usr/include/standards.h
vmat_matmat$(BIN_EXT).o: ../include/libvecmat.h ../include/stdtypes.h
vmat_matmat$(BIN_EXT).o: ../include/stdext.h /usr/include/math.h /usr/include/sgidefs.h
vmat_matmat$(BIN_EXT).o: /usr/include/standards.h
vmat_negate$(BIN_EXT).o: ../include/libvecmat.h ../include/stdtypes.h
vmat_negate$(BIN_EXT).o: ../include/stdext.h /usr/include/math.h /usr/include/sgidefs.h
vmat_negate$(BIN_EXT).o: /usr/include/standards.h
vmat_project$(BIN_EXT).o: ../include/libvecmat.h ../include/stdtypes.h
vmat_project$(BIN_EXT).o: ../include/stdext.h /usr/include/math.h
vmat_project$(BIN_EXT).o: /usr/include/sgidefs.h /usr/include/standards.h
vmat_rotate$(BIN_EXT).o: ../include/libvecmat.h ../include/stdtypes.h
vmat_rotate$(BIN_EXT).o: ../include/stdext.h /usr/include/math.h /usr/include/sgidefs.h
vmat_rotate$(BIN_EXT).o: /usr/include/standards.h
vmat_scal$(BIN_EXT).o: ../include/libvecmat.h ../include/stdtypes.h ../include/stdext.h
vmat_scal$(BIN_EXT).o: /usr/include/math.h /usr/include/sgidefs.h
vmat_scal$(BIN_EXT).o: /usr/include/standards.h
vmat_trans$(BIN_EXT).o: ../include/libvecmat.h ../include/stdtypes.h
vmat_trans$(BIN_EXT).o: ../include/stdext.h /usr/include/math.h /usr/include/sgidefs.h
vmat_trans$(BIN_EXT).o: /usr/include/standards.h
vmat_unit$(BIN_EXT).o: ../include/libvecmat.h ../include/stdtypes.h ../include/stdext.h
vmat_unit$(BIN_EXT).o: /usr/include/math.h /usr/include/sgidefs.h
vmat_unit$(BIN_EXT).o: /usr/include/standards.h
vmat_vecmat$(BIN_EXT).o: ../include/libvecmat.h ../include/stdtypes.h
vmat_vecmat$(BIN_EXT).o: ../include/stdext.h /usr/include/math.h /usr/include/sgidefs.h
vmat_vecmat$(BIN_EXT).o: /usr/include/standards.h
