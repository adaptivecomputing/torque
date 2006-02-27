include $(top_srcdir)/buildutils/config.mk

AM_CPPFLAGS += -DPBS_MOM -DDEMUX=\"$(DEMUX_PATH)\" -DRCP_PATH=\"$(RCP_PATH)\"

noinst_LIBRARIES = libfoo.a

libfoo_a_SOURCES = mom_mach.c mom_mach.h mom_start.c pe_input.c
