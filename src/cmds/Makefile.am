include $(top_srcdir)/buildutils/config.mk

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

qmgr_LDADD = $(PBS_LIBS) $(READLINE_LIBS)

pbspoe_CFLAGS = -DPBSPD=\"$(bindir)/pbspd\"
qsub_CFLAGS = -DSUBMIT_FILTER_PATH=\"$(SUBMIT_FILTER_PATH)\" \
              -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\"

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
qchkpt_SOURCES = qchkpt.c
qgpumode_SOURCES = qgpumode.c
qgpureset_SOURCES = qgpureset.c
pbs_track_SOURCES = pbs_track.c

CLEANFILES = $(dist_bin_SCRIPTS)

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
