include $(top_srcdir)/buildutils/config.mk

SUBDIRS = samples

PBS_LIBS = ../lib/Libpbs/libtorque.la

sbin_PROGRAMS = pbs_sched

pbs_sched_LDADD = samples/@SCHD_CODE@/libfoo.la $(PBS_LIBS) 
AM_CFLAGS = -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" -DPBS_ENVIRON=\"$(PBS_ENVIRON)\"

pbs_sched_SOURCES = pbs_sched.c get_4byte.c

install-exec-hook:
	chmod 700 $(DESTDIR)$(sbindir)/$(program_prefix)pbs_sched$(program_suffix)

