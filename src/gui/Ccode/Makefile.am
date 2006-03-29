include $(top_srcdir)/buildutils/config.mk

PBS_LIBS = ../../lib/Libpbs/libtorque.la

xpbsbindir = $(XPBS_DIR)/bin

xpbsbin_PROGRAMS = xpbs_datadump xpbs_scriptload

LDADD = $(PBS_LIBS)

xpbs_datadump_SOURCES = xpbs_datadump.c
xpbs_scriptload_SOURCES = xpbs_scriptload.c
