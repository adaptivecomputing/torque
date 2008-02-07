include $(top_srcdir)/buildutils/config.mk


AM_CPPFLAGS += -DPBS_MOM -DDEMUX=\"$(DEMUX_PATH)\" 

noinst_LIBRARIES = libmommach.a

libmommach_a_SOURCES = mom_mach.c mom_mach.h mom_start.c pe_input.c
if BUILD_L26_CPUSETS
libmommach_a_SOURCES += cpuset.c
endif
