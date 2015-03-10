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
CLEANFILES = *.gcda *.gcno *.gcov

tracejob_CFLAGS = -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\"
printserverdb_CFLAGS = -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\"
tracejob_CPPFLAGS = $(AM_CXXFLAGS) -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\"
printserverdb_CPPFLAGS = $(AM_CXXFLAGS) -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\"

chk_tree_SOURCES = chk_tree.cpp
hostn_SOURCES = hostn.cpp
printjob_SOURCES = printjob.cpp
printtracking_SOURCES = printtracking.cpp
printserverdb_SOURCES = printserverdb.cpp
tracejob_SOURCES = tracejob.cpp

pbs_tclsh_LDADD = $(PBS_LIBS) $(MY_TCL_LIBS)
pbs_tclsh_CFLAGS = $(MY_TCL_INCS)
pbs_tclsh_CPPFLAGS = $(AM_CXXFLAGS) $(MY_TCL_INCS)
pbs_tclsh_SOURCES = pbsTclInit.cpp ../scheduler.tcl/pbs_tclWrap.cpp \
		    ../scheduler.tcl/site_tclWrap.cpp

pbs_wish_LDADD = $(PBS_LIBS) $(MY_TCLTK_LIBS)
pbs_wish_CFLAGS = $(MY_TCLTK_INCS)
pbs_wish_CPPFLAGS = $(AM_CXXFLAGS) $(MY_TCLTK_INCS)
pbs_wish_SOURCES = pbsTkInit.cpp ../scheduler.tcl/pbs_tclWrap.cpp \
		   ../scheduler.tcl/site_tclWrap.cpp

install_gui:
	for dir in $(XPBSMON) ;do (cd $$dir && $(MAKE) install);done

if HAVE_CHECK
check:

.PHONY: cleancheck
cleancheck:
	cd test && $(MAKE) cleancheck
endif
