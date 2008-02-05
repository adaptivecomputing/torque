include $(top_srcdir)/buildutils/config.mk

if INCLUDE_GUI
if USING_TK
XPBSMON = xpbsmon
endif
endif

SUBDIRS = . $(XPBSMON)

DIST_SUBDIRS = . xpbsmon

EXTRA_DIST = tracejob.h init.d/pbs

PBS_LIBS = ../lib/Libpbs/libtorque.la

if USING_TCL
PROGRAMS_TCL = pbs_tclsh
endif

if INCLUDE_GUI
if USING_TK
PROGRAMS_TK = pbs_wish
endif
endif

bin_PROGRAMS = chk_tree hostn printjob printtracking printserverdb tracejob $(PROGRAMS_TCL) $(PROGRAMS_TK)

LDADD = $(PBS_LIBS)

tracejob_CFLAGS = -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\"
printserverdb_CFLAGS = -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\"

chk_tree_SOURCES = chk_tree.c
hostn_SOURCES = hostn.c
printjob_SOURCES = printjob.c
printtracking_SOURCES = printtracking.c
printserverdb_SOURCES = printserverdb.c
tracejob_SOURCES = tracejob.c

pbs_tclsh_LDADD = $(PBS_LIBS) $(MY_TCL_LIBS)
pbs_tclsh_CFLAGS = $(MY_TCL_INCS)
pbs_tclsh_SOURCES = pbsTclInit.c ../scheduler.tcl/pbs_tclWrap.c \
		    ../scheduler.tcl/site_tclWrap.c

pbs_wish_LDADD = $(PBS_LIBS) $(MY_TCLTK_LIBS)
pbs_wish_CFLAGS = $(MY_TCLTK_INCS)
pbs_wish_SOURCES = pbsTkInit.c ../scheduler.tcl/pbs_tclWrap.c \
		   ../scheduler.tcl/site_tclWrap.c

install_gui:
	for dir in $(XPBSMON) ;do (cd $$dir && $(MAKE) install);done

