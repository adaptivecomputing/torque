include $(top_srcdir)/buildutils/config.mk

PBS_LIBS = ../lib/Libnet/libnet.la \
	   ../lib/Libpbs/libpbs.la

sbin_PROGRAMS = pbs_iff

pbs_iff_LDADD = $(PBS_LIBS)

pbs_iff_SOURCES = iff2.c

install-exec-hook:
	chmod u+s $(DESTDIR)$(sbindir)/pbs_iff
