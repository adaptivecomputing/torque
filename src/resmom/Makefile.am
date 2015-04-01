include $(top_srcdir)/buildutils/config.mk

CHECK_DIR = test
SUBDIRS = @PBS_MACH@

DIST_SUBDIRS = aix4 aix5 cygwin darwin digitalunix freebsd freebsd5 fujitsu \
	hpux10 hpux11 irix5 irix6 irix6array linux netbsd solaris5 solaris7 sunos4 \
	unicos8 unicosmk2

CLEANFILES = *.gcda *.gcno *.gcov

include_HEADERS = catch_child.h checkpoint.h mom_comm.h mom_main.h mom_process_request.h \
		mom_server_lib.h mom_job_func.h cray_energy.h

AM_CXXFLAGS = -I$(top_srcdir)/src/resmom/@PBS_MACH@ -DPBS_MOM \
	       -DDEMUX=\"$(program_prefix)$(DEMUX_PATH)$(program_suffix)\" \
	       -DRCP_PATH=\"$(program_prefix)$(RCP_PATH)$(program_suffix)\" \
	       -DRCP_ARGS=\"$(RCP_ARGS)\" \
	       -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" -DPBS_ENVIRON=\"$(PBS_ENVIRON)\" \
	       `xml2-config --cflags`
AM_LIBS   = `xml2-config --libs`

PBS_LIBS = ../lib/Libattr/libattr.a \
	   ../lib/Libsite/libsite.a \
		 ../lib/Libutils/libutils.a \
	   ../lib/Libpbs/libtorque.la

EXTRA_DIST = rm_dep.h
sbin_PROGRAMS = pbs_mom pbs_demux

LDADD = @PBS_MACH@/libmommach.a $(MOMLIBS) $(PBS_LIBS) $(ALPS_LIBS)

pbs_mom_SOURCES = catch_child.cpp mom_comm.cpp mom_inter.cpp mom_main.cpp	\
		   mom_server.cpp prolog.cpp requests.cpp start_exec.cpp	\
		   start_exec.h checkpoint.cpp tmsock_recov.cpp		\
		   mom_req_quejob.cpp mom_job_func.cpp			\
		   mom_process_request.cpp alps_reservations.cpp		\
		   release_reservation.cpp generate_alps_status.cpp	\
		   parse_config.cpp node_frequency.cpp cray_energy.cpp \
		   ../server/attr_recov.cpp ../server/dis_read.cpp		\
		   ../server/job_attr_def.cpp ../server/job_recov.cpp	\
		   ../server/reply_send.cpp ../server/resc_def_all.cpp	\
		   ../server/job_qs_upgrade.cpp
if BUILDCPA
pbs_mom_SOURCES += cray_cpa.cpp
endif

if MIC
pbs_mom_SOURCES += mic.cpp
endif

if NVIDIA
pbs_mom_SOURCES += nvidia.cpp
endif

pbs_demux_SOURCES = pbs_demux.cpp
# use LDADD here so that we don't have to link MOM with libcr
pbs_demux_LDADD = $(BLCR_LDFLAGS) @PBS_MACH@/libmommach.a $(MOMLIBS) $(PBS_LIBS)
pbs_demux_CPPFLAGS = $(BLCR_CPPFLAGS) $(AM_CXXFLAGS)

install-exec-hook:
	$(PBS_MKDIRS) aux || :
	$(PBS_MKDIRS) mom
	rm -f $(DESTDIR)$(sbindir)/$(program_prefix)qnoded$(program_suffix)$(EXEEXT)
	ln -s $(program_prefix)pbs_mom$(program_suffix)$(EXEEXT) \
	  $(DESTDIR)$(sbindir)/$(program_prefix)qnoded$(program_suffix)$(EXEEXT)

uninstall-hook:
	rm -f $(DESTDIR)$(PBS_ENVIRON)
	rm -f $(DESTDIR)$(sbindir)/$(program_prefix)qnoded$(program_suffix)$(EXEEXT)

if HAVE_CHECK
check:
	$(MAKE) -C $(CHECK_DIR) $(MAKECMDGOALS)

.PHONY: cleancheck
cleancheck:
	cd test && $(MAKE) cleancheck
endif
