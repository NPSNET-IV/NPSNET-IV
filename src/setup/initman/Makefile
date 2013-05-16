# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	=	Makefile

# You can change these flags while testing for optimizations
#OPT	=	-O2 -Olimit 1220
OPT = -O2 $(OLIMIT_DEF)
#CFLAGS	=	$(OPT)

# You can locally define any symbols you need using the next line
#DEFINES	= -DSEARCH_PATH_TRACE

# You can define include directories your code needs here --
#    other than ../include
APPINCDIRS	= -I../include \
		  -I../../communication/include \
		  -I../../display/include \
		  -I../../environment/include \
		  -I../../database/include \
		  -I../../interface/include \
		  -I../../entities/include \
		  -I../../apps/include \
		  -I../../c2/include \
		  -I../../setup/include \
		  -I../../utilities/include \
		  -I/usr/include/Performer \
		  -I/usr/src/Performer/include

HFILES		= ../include/im.tab.h ../include/imstructs.h \
		  ../include/imdefaults.h ../include/imclass.h \
		  ../include/imextern.h ../include/imsymbols.h

PDU		= ../../communication/include/pdu.h
SETUP		= ../../setup/include/setup_const.h

OTHERS		= ../../display/include/display_const.h \
		  ../../environment/include/environment_const.h \
		  ../../database/include/database_const.h \
		  ../../entities/include/entity_const.h \
		  ../../interface/include/interface_const.h

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	y.tab.c lex.yy.c initman.cc cl_args.cc env_var.cc \
		query.cc dumps.cc

TARGETS =       $(LIBDIR)libinitman$(BIN_EXT).$(LIB_EXT)

OBJECTS	=	y.tab$(BIN_EXT).o \
		lex.yy$(BIN_EXT).o \
		initman$(BIN_EXT).o \
		cl_args$(BIN_EXT).o \
		env_var$(BIN_EXT).o \
		query$(BIN_EXT).o \
		dumps$(BIN_EXT).o


# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post

lex.yy.c: lexer.l $(HFILES)
	lex lexer.l
	sed -f lex.sed lex.yy.c > tmp
	mv tmp lex.yy.c

y.tab.c: parser.y ../include/imstructs.h ../include/imdefaults.h \
	../include/imsymbols.h \
	$(PDU) $(SETUP)
	yacc -d parser.y
	mv y.tab.h ../include/im.tab.h

clobber: other_clobber

other_clobber:
	@ set nonomatch;rm -f y.tab.c ../include/im.tab.h

# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

lex$(BIN_EXT).yy.o: mangle.h ../include/imstructs.h ../../communication/include/pdu.h
lex$(BIN_EXT).yy.o: ../../communication/include/envirvalues.h ../include/setup_const.h
lex$(BIN_EXT).yy.o: ../include/imsymbols.h ../include/imdefaults.h
lex$(BIN_EXT).yy.o: ../../communication/include/communication_const.h
initman$(BIN_EXT).o: ../include/imextern.h ../include/imclass.h ../include/imstructs.h
initman$(BIN_EXT).o: ../../communication/include/pdu.h
initman$(BIN_EXT).o: ../../communication/include/envirvalues.h ../include/setup_const.h
initman$(BIN_EXT).o: ../include/imdefaults.h
initman$(BIN_EXT).o: ../../communication/include/communication_const.h
initman$(BIN_EXT).o: ../../display/include/display_const.h
initman$(BIN_EXT).o: ../../environment/include/environment_const.h
initman$(BIN_EXT).o: ../../database/include/database_const.h
initman$(BIN_EXT).o: ../../entities/include/entity_const.h
initman$(BIN_EXT).o: ../../utilities/include/conversion_const.h
initman$(BIN_EXT).o: ../../interface/include/interface_const.h
cl_args$(BIN_EXT).o: ../include/imstructs.h ../../communication/include/pdu.h
cl_args$(BIN_EXT).o: ../../communication/include/envirvalues.h ../include/setup_const.h
cl_args$(BIN_EXT).o: ../include/imdefaults.h
cl_args$(BIN_EXT).o: ../../communication/include/communication_const.h
cl_args$(BIN_EXT).o: ../include/imextern.h ../include/imclass.h
env_var$(BIN_EXT).o: ../include/imstructs.h ../../communication/include/pdu.h
env_var$(BIN_EXT).o: ../../communication/include/envirvalues.h ../include/setup_const.h
env_var$(BIN_EXT).o: ../include/imdefaults.h
env_var$(BIN_EXT).o: ../../communication/include/communication_const.h
env_var$(BIN_EXT).o: ../include/imextern.h ../include/imclass.h
query$(BIN_EXT).o: ../include/imstructs.h ../../communication/include/pdu.h
query$(BIN_EXT).o: ../../communication/include/envirvalues.h ../include/setup_const.h
query$(BIN_EXT).o: ../include/imdefaults.h
query$(BIN_EXT).o: ../../communication/include/communication_const.h
query$(BIN_EXT).o: ../include/imextern.h ../include/imclass.h
dumps$(BIN_EXT).o: ../include/imstructs.h ../../communication/include/pdu.h
dumps$(BIN_EXT).o: ../../communication/include/envirvalues.h ../include/setup_const.h
dumps$(BIN_EXT).o: ../include/imdefaults.h
dumps$(BIN_EXT).o: ../../communication/include/communication_const.h
dumps$(BIN_EXT).o: ../include/imextern.h ../include/imclass.h
