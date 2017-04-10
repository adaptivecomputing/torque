include ../Makefile_Client.ut

if USE_TCLQSTAT
TCLQSTATLIBS = $(MY_TCL_LIBS)
TCLQSTATCFLAGS = $(MY_TCL_INCS)
endif

libuut_la_SOURCES = ${PROG_ROOT}/qstat.c
libuut_la_LDFLAGS += $(TCL_LIB_FLAG)
libuut_CFLAGS = $(TCLQSTATCFLAGS)

