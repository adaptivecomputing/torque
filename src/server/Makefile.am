include $(top_srcdir)/buildutils/config.mk

if HAVE_CHECK
SUBDIRS = test
endif

CLEANFILES = *.gcda *.gcno *.gcov

DIST_SUBDIRS=

include_HEADERS = array_func.h issue_request.h job_func.h node_func.h node_manager.h pbsd_main.h process_request.h queue_func.h queue_recov.h pbsd_init.h reply_send.h req_delete.h req_deletearray.h req_getcred.h req_gpuctrl.h req_holdarray.h req_holdjob.h req_jobobit.h req_locate.h req_manager.h req_message.h req_modify.h req_movejob.h req_quejob.h req_register.h req_rerun.h req_rescq.h req_runjob.h req_select.h req_shutdown.h req_signal.h req_stat.h req_track.h svr_connect.h svr_jobfunc.h queue_recycler.h svr_movejob.h svr_task.h svr_func.h ji_mutex.h job_route.h

PBS_LIBS = ../lib/Libattr/libattr.a \
	   ../lib/Libsite/libsite.a \
		 ../lib/Libutils/libutils.a \
		 ../lib/Libifl/libifl.a \
	   ../lib/Libpbs/libtorque.la

sbin_PROGRAMS = pbs_server

pbs_server_LDADD = $(PBS_LIBS) $(ALPS_LIBS)
AM_CFLAGS = -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" -DPBS_ENVIRON=\"$(PBS_ENVIRON)\" `xml2-config --cflags`
AM_LIBS   =`xml2-config --libs`

pbs_server_SOURCES = accounting.c array_func.c array_upgrade.c attr_recov.c \
		     dis_read.c geteusernam.c \
		     issue_request.c job_attr_def.c job_func.c job_recov.c \
		     job_route.c node_attr_def.c node_func.c \
		     node_manager.c pbsd_init.c pbsd_main.c \
		     process_request.c queue_attr_def.c queue_func.c \
		     queue_recov.c reply_send.c req_delete.c \
		     req_deletearray.c req_getcred.c req_gpuctrl.c \
		     req_holdjob.c req_jobobit.c req_locate.c req_manager.c \
		     req_message.c req_modify.c req_movejob.c req_quejob.c \
		     req_register.c req_rerun.c req_rescq.c req_runjob.c \
		     req_select.c req_shutdown.c req_signal.c req_stat.c \
		     req_track.c resc_def_all.c run_sched.c stat_job.c \
		     svr_attr_def.c svr_chk_owner.c svr_connect.c \
		     svr_func.c svr_jobfunc.c svr_mail.c svr_movejob.c \
		     svr_recov.c svr_resccost.c svr_task.c req_tokens.c \
		     job_qs_upgrade.c req_holdarray.c svr_format_job.c \
				 job_recycler.c queue_recycler.c process_alps_status.c \
				 display_alps_status.c login_nodes.c track_alps_reservations.c \
				 batch_request.c user_info.c exiting_jobs.c

install-exec-hook:
	$(PBS_MKDIRS) aux || :
	$(PBS_MKDIRS) server
	rm -f $(DESTDIR)$(sbindir)/$(program_prefix)qserverd$(program_suffix)$(EXEEXT)
	ln -s $(program_prefix)pbs_server$(program_suffix)$(EXEEXT) \
	   $(DESTDIR)$(sbindir)/$(program_prefix)qserverd$(program_suffix)$(EXEEXT)

uninstall-hook:
	rm -f $(DESTDIR)$(PBS_ENVIRON)
	rm -f $(DESTDIR)$(sbindir)/$(program_prefix)qserverd$(program_suffix)$(EXEEXT)
