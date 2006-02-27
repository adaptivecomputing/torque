AUTOMAKE_OPTIONS = foreign

PBS_MKDIRS = $(SHELL) $(top_builddir)/buildutils/pbs_mkdirs

AM_CPPFLAGS = -I$(top_srcdir)/src/include

IFF_PATH = $(sbindir)/pbs_iff
DEMUX_PATH = $(sbindir)/pbs_demux
RCP_PATH = $(sbindir)/pbs_rcp

XPBS_DIR = $(libdir)/xpbs
XPBSMON_DIR = $(libdir)/xpbsmon
