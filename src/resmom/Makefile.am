include $(top_srcdir)/buildutils/config.mk

SUBDIRS = @PBS_MACH@
DIST_SUBDIRS = aix4 aix5 darwin digitalunix freebsd fujitsu hpux10 hpux11 \
	irix5 irix6 irix6array linux netbsd solaris5 solaris7 sunos4 unicos8 unicosmk2

AM_CPPFLAGS += -I$(top_srcdir)/src/resmom/@PBS_MACH@ \
	       -DPBS_MOM -DDEMUX=\"$(DEMUX_PATH)\" -DRCP_PATH=\"$(RCP_PATH)\" -DRCP_ARGS=\"$(RCP_ARGS)\" \
	       -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" -DPBS_ENVIRON=\"$(PBS_ENVIRON)\"

PBS_LIBS = ../lib/Libattr/libattr.a \
	   ../lib/Liblog/liblog.la \
	   ../lib/Libnet/libnet.la \
	   ../lib/Libpbs/libpbs.la \
	   ../lib/Libsite/libsite.a

EXTRA_DIST = rm_dep.h
sbin_PROGRAMS = pbs_mom

LDADD = $(PBS_LIBS) $(MOMLIBS) @PBS_MACH@/libfoo.a

pbs_mom_SOURCES = catch_child.c mom_comm.c mom_inter.c mom_main.c \
		   mom_server.c prolog.c requests.c start_exec.c tmsock_recov.c \
		   ../server/attr_recov.c ../server/dis_read.c \
		   ../server/job_attr_def.c ../server/job_func.c \
		   ../server/job_recov.c  \
		   ../server/process_request.c ../server/reply_send.c \
		   ../server/req_quejob.c ../server/resc_def_all.c

install-exec-hook:
	chmod 700 $(DESTDIR)$(sbindir)/pbs_mom
	$(PBS_MKDIRS) aux
	$(PBS_MKDIRS) mom

uninstall-hook:
	rm -f $(DESTDIR)$(PBS_ENVIRON)

