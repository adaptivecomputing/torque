include $(top_srcdir)/buildutils/config.mk

if INCLUDE_GUI
if USING_TK
XPBSMON = xpbsmon
endif
endif

SUBDIRS = . $(XPBSMON)

DIST_SUBDIRS = . xpbsmon

EXTRA_DIST = tracejob.h init.d/pbs

PBS_LIBS = ../lib/Libcmds/libcmds.la \
	   ../lib/Liblog/liblog.la \
	   ../lib/Libnet/libnet.la \
	   ../lib/Libpbs/libpbs.la

if USING_TCL
PROGRAMS_TCL = pbs_tclsh
endif

if INCLUDE_GUI
if USING_TK
PROGRAMS_TK = pbs_wish
endif
endif

bin_PROGRAMS = chk_tree hostn printjob printtracking tracejob $(PROGRAMS_TCL) $(PROGRAMS_TK)

LDADD = $(PBS_LIBS)
AM_CFLAGS = $(AM_CPPFLAGS) -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" $(MY_TCL_INCS)

chk_tree_SOURCES = chk_tree.c
hostn_SOURCES = hostn.c
printjob_SOURCES = printjob.c
printtracking_SOURCES = printtracking.c
tracejob_SOURCES = tracejob.c

pbs_tclsh_LDADD = $(PBS_LIBS) $(MY_TCL_LIBS)
pbs_tclsh_SOURCES = pbsTclInit.c ../scheduler.tcl/pbs_tclWrap.c \
		    ../scheduler.tcl/site_tclWrap.c

pbs_wish_LDADD = $(PBS_LIBS) $(MY_TCL_LIBS)
pbs_wish_SOURCES = pbsTkInit.c ../scheduler.tcl/pbs_tclWrap.c \
		   ../scheduler.tcl/site_tclWrap.c

install_gui:
	for dir in $(XPBSMON) ;do (cd $$dir && $(MAKE) install);done

