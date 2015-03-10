include $(top_srcdir)/buildutils/config.mk

CHECK_DIR = test

DIST_SUBDIRS=

include_HEADERS = common_cmds.h pbsnodes.h qsub_functions.h
PBS_LIBS = ../lib/Libpbs/libtorque.la \
					 ../lib/Libutils/libutils.a

dist_bin_SCRIPTS = nqs2pbs
bin_PROGRAMS = qalter qdel qhold qmove qorder qmsg qrerun qrls qselect qsig \
	       qstat qsub pbsdsh qdisable qenable qrun qstart qstop qterm \
	       pbsnodes qmgr qchkpt qgpumode qgpureset pbs_track @PBSPOE@
EXTRA_PROGRAMS = pbspd pbspoe
EXTRA_DIST = nqs2pbs.in sample.qstatrc

LDADD = $(PBS_LIBS)

if USE_TCLQSTAT
TCLQSTATLIBS = $(MY_TCL_LIBS)
TCLQSTATCFLAGS = $(MY_TCL_INCS)
endif
qstat_LDADD = $(PBS_LIBS) $(TCLQSTATLIBS)
qstat_CFLAGS = $(TCLQSTATCFLAGS)
qstat_CPPFLAGS = $(AM_CXXFLAGS) $(TCLQSTATCFLAGS)

qmgr_LDADD = $(PBS_LIBS) $(READLINE_LIBS)

pbspoe_CFLAGS = -DPBSPD=\"$(bindir)/pbspd\"
pbspoe_CPPFLAGS = $(AM_CXXFLAGS) -DPBSPD=\"$(bindir)/pbspd\"

qsub_CFLAGS = -DSUBMIT_FILTER_PATH=\"$(SUBMIT_FILTER_PATH)\" \
              -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\"
qsub_CPPFLAGS = $(AM_CXXFLAGS) -DSUBMIT_FILTER_PATH=\"$(SUBMIT_FILTER_PATH)\" \
              -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\"

pbsdsh_SOURCES = pbsdsh.cpp
pbsnodes_SOURCES = pbsnodes.cpp MXML.cpp
pbspd_SOURCES = pbspd.cpp
pbspoe_SOURCES = pbspoe.cpp
qalter_SOURCES = qalter.cpp
qdel_SOURCES = qdel.cpp common_cmds.cpp
qdisable_SOURCES = qdisable.cpp
qenable_SOURCES = qenable.cpp
qhold_SOURCES = qhold.cpp
qmgr_SOURCES = qmgr.cpp
qmove_SOURCES = qmove.cpp
qmsg_SOURCES = qmsg.cpp
qorder_SOURCES = qorder.cpp
qrerun_SOURCES = qrerun.cpp
qrls_SOURCES = qrls.cpp
qrun_SOURCES = qrun.cpp
qselect_SOURCES = qselect.cpp
qsig_SOURCES = qsig.cpp
qstart_SOURCES = qstart.cpp
qstat_SOURCES = qstat.cpp MXML.cpp
qstop_SOURCES = qstop.cpp
qsub_SOURCES = qsub.cpp qsub_functions.cpp common_cmds.cpp
#qsub_LDFLAGS = -all-static
qterm_SOURCES = qterm.cpp
qchkpt_SOURCES = qchkpt.cpp
qgpumode_SOURCES = qgpumode.cpp
qgpureset_SOURCES = qgpureset.cpp
pbs_track_SOURCES = pbs_track.cpp

CLEANFILES = $(dist_bin_SCRIPTS)
CLEANFILES += *.gcda *.gcno *.gcov

do_subst = sed -e 's,[@]AWK[@],$(AWK),g'

nqs2pbs: nqs2pbs.in Makefile
	$(do_subst) < $(srcdir)/nqs2pbs.in > nqs2pbs
	chmod +x nqs2pbs


install-exec-hook:
	$(PBS_MKDIRS) aux || :
	$(PBS_MKDIRS) default
	rm -f $(DESTDIR)$(bindir)/$(program_prefix)qnodes$(program_suffix)$(EXEEXT)
	ln -s $(program_prefix)pbsnodes$(program_suffix)$(EXEEXT) \
	  $(DESTDIR)$(bindir)/$(program_prefix)qnodes$(program_suffix)$(EXEEXT)

uninstall-hook:
	rm -f $(DESTDIR)$(PBS_DEFAULT_FILE)
	rm -f $(DESTDIR)$(bindir)/$(program_prefix)qnodes$(program_suffix)$(EXEEXT)

if HAVE_CHECK
check:
	$(MAKE) -C $(CHECK_DIR) $(MAKECMDGOALS)

.PHONY: cleancheck
cleancheck:
	cd test && $(MAKE) cleancheck
endif
