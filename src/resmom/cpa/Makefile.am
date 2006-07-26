include $(top_srcdir)/buildutils/config.mk

noinst_LIBRARIES = libmomcpa.a

AM_CPPFLAGS += -DPBS_MOM -I$(top_srcdir)/src/resmom/@PBS_MACH@

libmomcpa_a_SOURCES = cray_cpa.c
