include $(top_srcdir)/buildutils/config.mk

PBS_LIBS = ../lib/Libpbs/libtorque.la

bin_SCRIPTS = nqs2pbs
sbin_PROGRAMS = pbs_demux momctl
bin_PROGRAMS = qalter qdel qhold qmove qorder qmsg qrerun qrls qselect qsig \
	       qstat qsub pbsdsh qdisable qenable qrun qstart qstop qterm \
	       pbsnodes qmgr @PBSPOE@
EXTRA_PROGRAMS = pbspd pbspoe
EXTRA_DIST = nqs2pbs sample.qstatrc

LDADD = $(PBS_LIBS)

AM_CFLAGS = $(MY_TCL_INCS) -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" \
	    -DPBSPD=\"$(bindir)/pbspd\"

if USE_TCLQSTAT
TCLQSTATLIBS = $(MY_TCL_LIBS)
endif
qstat_LDADD = $(PBS_LIBS) $(TCLQSTATLIBS)

qmgr_LDADD = $(PBS_LIBS) $(READLINE_LIBS)

pbs_demux_SOURCES = pbs_demux.c
pbsdsh_SOURCES = pbsdsh.c
pbsnodes_SOURCES = pbsnodes.c MXML.c
pbspd_SOURCES = pbspd.c
pbspoe_SOURCES = pbspoe.c
qalter_SOURCES = qalter.c
qdel_SOURCES = qdel.c
qdisable_SOURCES = qdisable.c
qenable_SOURCES = qenable.c
qhold_SOURCES = qhold.c
qmgr_SOURCES = qmgr.c
qmove_SOURCES = qmove.c
qmsg_SOURCES = qmsg.c
qorder_SOURCES = qorder.c
qrerun_SOURCES = qrerun.c
qrls_SOURCES = qrls.c
qrun_SOURCES = qrun.c
qselect_SOURCES = qselect.c
qsig_SOURCES = qsig.c
qstart_SOURCES = qstart.c
qstat_SOURCES = qstat.c MXML.c
qstop_SOURCES = qstop.c
qsub_SOURCES = qsub.c
qterm_SOURCES = qterm.c
momctl_SOURCES = momctl.c

install-exec-hook:
	$(PBS_MKDIRS) aux
	$(PBS_MKDIRS) default

uninstall-hook:
	rm -f $(DESTDIR)$(PBS_DEFAULT_FILE)
