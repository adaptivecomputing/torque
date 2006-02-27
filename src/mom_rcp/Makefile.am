include $(top_srcdir)/buildutils/config.mk

sbin_PROGRAMS = pbs_rcp

pbs_rcp_SOURCES = rcp.c replace.c util.c extern.h  pathnames.h

EXTRA_DIST = ReadMe

install-exec-hook:
	chmod u+s $(DESTDIR)$(sbindir)/pbs_rcp
