include $(top_srcdir)/buildutils/config.mk

SUBDIRS = samples

PBS_LIBS = ../lib/Liblog/liblog.la \
	   ../lib/Libnet/libnet.la \
	   ../lib/Libpbs/libpbs.la

sbin_PROGRAMS = pbs_sched

pbs_sched_LDADD = samples/@SCHD_CODE@/libfoo.la $(PBS_LIBS) 
pbs_sched_SOURCES = pbs_sched.c get_4byte.c

install-exec-hook:
	chmod 700 $(DESTDIR)$(sbindir)/pbs_sched
