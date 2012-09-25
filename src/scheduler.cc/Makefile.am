include $(top_srcdir)/buildutils/config.mk

#if HAVE_CHECK
#SUBDIRS = samples test
#else
SUBDIRS = samples
#endif

PBS_LIBS = ../lib/Libpbs/libtorque.la

sbin_PROGRAMS = pbs_sched

CLEANFILES = *.gcda *.gcno *.gcov
CLEANFILES += samples/@SCHD_CODE@/*.gcda samples/@SCHD_CODE@/*.gcno samples/@SCHD_CODE@/*.gcov

pbs_sched_LDADD = samples/@SCHD_CODE@/libfoo.la $(PBS_LIBS) 
AM_CFLAGS = -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" -DPBS_ENVIRON=\"$(PBS_ENVIRON)\"

pbs_sched_SOURCES = pbs_sched.c get_4byte.c

install-exec-hook:
	rm -f $(DESTDIR)$(sbindir)/$(program_prefix)qschedd$(program_suffix)$(EXEEXT)
	ln -s $(program_prefix)pbs_sched$(program_suffix)$(EXEEXT) \
	  $(DESTDIR)$(sbindir)/$(program_prefix)qschedd$(program_suffix)$(EXEEXT)

uninstall-hook:
	rm -f $(DESTDIR)$(sbindir)/$(program_prefix)qschedd$(program_suffix)$(EXEEXT)

