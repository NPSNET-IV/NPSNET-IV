# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_pre

MAKEFILE	= Makefile

# You can change these flags while testing for optimizations
OPT	=	-O2
#OPT	=	-g
#CFLAGS	=	$(OPT)

# You can locally define any symbols you need using the next line
#DEFINES       =       -DCHECK_IFF_FLAGS

# You can define include directories your code needs here --
#    other than ../include
#APPINCDIRS	=

# ***********************************************************************
# ***** ALWAYS list targets and the objects for the targets here *****

SOURCES	=	sockcomm.cc cl_sockcomm.cc ip_sockcomm.cc up_sockcomm.cc \
		ip_cl_sockcomm.cc mc_ip_cl_sockcomm.cc bc_ip_cl_sockcomm.cc \
		uc_ip_cl_sockcomm.cc co_sockcomm.cc ip_co_sockcomm.cc \
		up_co_sockcomm.cc alarmclock.cc up_cl_sockcomm.cc
TARGETS	=	$(LIBDIR)libsockcomm$(BIN_EXT).$(LIB_EXT)
OBJECTS	=	sockcomm$(BIN_EXT).o \
		cl_sockcomm$(BIN_EXT).o \
		ip_sockcomm$(BIN_EXT).o \
		up_sockcomm$(BIN_EXT).o \
		ip_cl_sockcomm$(BIN_EXT).o \
		mc_ip_cl_sockcomm$(BIN_EXT).o \
		bc_ip_cl_sockcomm$(BIN_EXT).o \
		uc_ip_cl_sockcomm$(BIN_EXT).o \
		co_sockcomm$(BIN_EXT).o \
		ip_co_sockcomm$(BIN_EXT).o \
		up_co_sockcomm$(BIN_EXT).o \
		alarmclock$(BIN_EXT).o \
		up_cl_sockcomm$(BIN_EXT).o

# ***********************************************************************


# ***** ALWAYS leave the following include line ***
include ../../Makefile.lib_post


# ***********************************************************************
# DO NOT DELETE THIS LINE -- make depend depends on it.

sockcomm$(BIN_EXT).o: ../include/sockcomm.h
cl_sockcomm$(BIN_EXT).o: ../include/cl_sockcomm.h ../include/sockcomm.h
ip_sockcomm$(BIN_EXT).o: ../include/ip_sockcomm.h ../include/sockcomm.h
up_sockcomm$(BIN_EXT).o: ../include/up_sockcomm.h ../include/sockcomm.h
ip_cl_sockcomm$(BIN_EXT).o: ../include/ip_cl_sockcomm.h ../include/ip_sockcomm.h
ip_cl_sockcomm$(BIN_EXT).o: ../include/sockcomm.h ../include/cl_sockcomm.h
mc_ip_cl_sockcomm$(BIN_EXT).o: ../include/mc_ip_cl_sockcomm.h
mc_ip_cl_sockcomm$(BIN_EXT).o: ../include/ip_cl_sockcomm.h ../include/ip_sockcomm.h
mc_ip_cl_sockcomm$(BIN_EXT).o: ../include/sockcomm.h ../include/cl_sockcomm.h
bc_ip_cl_sockcomm$(BIN_EXT).o: ../include/bc_ip_cl_sockcomm.h
bc_ip_cl_sockcomm$(BIN_EXT).o: ../include/ip_cl_sockcomm.h ../include/ip_sockcomm.h
bc_ip_cl_sockcomm$(BIN_EXT).o: ../include/sockcomm.h ../include/cl_sockcomm.h
uc_ip_cl_sockcomm$(BIN_EXT).o: ../include/uc_ip_cl_sockcomm.h
uc_ip_cl_sockcomm$(BIN_EXT).o: ../include/ip_cl_sockcomm.h ../include/ip_sockcomm.h
uc_ip_cl_sockcomm$(BIN_EXT).o: ../include/sockcomm.h ../include/cl_sockcomm.h
co_sockcomm$(BIN_EXT).o: ../include/co_sockcomm.h ../include/sockcomm.h
ip_co_sockcomm$(BIN_EXT).o: ../include/ip_co_sockcomm.h ../include/ip_sockcomm.h
ip_co_sockcomm$(BIN_EXT).o: ../include/sockcomm.h ../include/co_sockcomm.h
ip_co_sockcomm$(BIN_EXT).o: ../include/alarmclock.h
up_co_sockcomm$(BIN_EXT).o: ../include/up_co_sockcomm.h ../include/up_sockcomm.h
up_co_sockcomm$(BIN_EXT).o: ../include/sockcomm.h ../include/co_sockcomm.h
up_co_sockcomm$(BIN_EXT).o: ../include/alarmclock.h
alarmclock$(BIN_EXT).o: ../include/alarmclock.h
up_cl_sockcomm$(BIN_EXT).o: ../include/up_cl_sockcomm.h ../include/up_sockcomm.h
up_cl_sockcomm$(BIN_EXT).o: ../include/sockcomm.h ../include/cl_sockcomm.h
up_cl_sockcomm$(BIN_EXT).o: ../include/alarmclock.h
