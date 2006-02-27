include $(top_srcdir)/buildutils/config.mk

AM_CPPFLAGS += -I$(top_srcdir)/src/resmom/@PBS_MACH@

noinst_LIBRARIES = libsite.a

libsite_a_SOURCES = site_allow_u.c site_alt_rte.c site_check_u.c \
		     site_map_usr.c site_mom_chu.c site_mom_ckp.c \
		     site_mom_jst.c
