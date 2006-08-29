AUTOMAKE_OPTIONS = foreign

PBS_MKDIRS = $(SHELL) $(top_builddir)/buildutils/pbs_mkdirs

AM_CPPFLAGS = -I$(top_srcdir)/src/include

IFF_PATH = $(sbindir)/$(program_prefix)pbs_iff$(program_suffix)
DEMUX_PATH = $(sbindir)/$(program_prefix)pbs_demux$(program_suffix)

XPBS_DIR = $(libdir)/xpbs
XPBSMON_DIR = $(libdir)/xpbsmon

SUBMIT_FILTER_PATH = $(libexecdir)/qsub_filter
