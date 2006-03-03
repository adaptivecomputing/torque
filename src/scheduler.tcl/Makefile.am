include $(top_srcdir)/buildutils/config.mk

PBS_LIBS = ../lib/Libcmds/libcmds.la \
	   ../lib/Liblog/liblog.la \
	   ../lib/Libnet/libnet.la \
	   ../lib/Libpbs/libpbs.la

sbin_PROGRAMS = pbs_sched

AM_CFLAGS = -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" -DPBS_ENVIRON=\"$(PBS_ENVIRON)\" $(MY_TCL_INCS)

pbs_sched_LDADD = $(PBS_LIBS) $(MY_TCL_LIBS)
pbs_sched_SOURCES = pbs_sched.c pbs_tclWrap.c site_tclWrap.c \
		    ../scheduler.cc/get_4byte.c

EXTRA_DIST = sample_scripts/README sample_scripts/loadave
install-exec-hook:
	chmod 700 $(DESTDIR)$(sbindir)/pbs_sched
