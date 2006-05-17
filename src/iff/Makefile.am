include $(top_srcdir)/buildutils/config.mk

PBS_LIBS = ../lib/Libpbs/libtorque.la

sbin_PROGRAMS = pbs_iff

pbs_iff_LDADD = $(PBS_LIBS)

pbs_iff_SOURCES = iff2.c

install-exec-hook:
	chmod u+s $(DESTDIR)$(sbindir)/$(program_prefix)pbs_iff$(program_suffix)

