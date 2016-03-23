include $(top_srcdir)/buildutils/config.mk

CLEANFILES = *.gcda *.gcno *.gcov

DIST_SUBDIRS=

noinst_HEADERS = accounting.h job_recov.h queue_recov.h req_manager.h req_select.h svr_connect.h \
                 array_func.h job_recycler.h queue_recycler.h req_message.h req_shutdown.h svr_format_job.h \
                 array_upgrade.h job_route.h reply_send.h req_modify.h req_signal.h svr_func.h \
                 attr_recov.h mom_hierarchy_handler.h req_deletearray.h req_modify_node.h req_stat.h svr_jobfunc.h \
                 completed_jobs_map.h node_func.h req_delete.h req_movejob.h req_tokens.h svr_mail.h \
                 dis_read.h node_manager.h req_getcred.h req_quejob.h req_track.h svr_movejob.h \
                 issue_request.h pbsd_init.h req_holdarray.h req_register.h resc_def_all.h svr_process_request.h \
                 ji_mutex.h pbsd_main.h req_holdjob.h req_rerun.h run_sched.h svr_recov.h job_func.h process_request.h \
                 req_jobobit.h req_rescq.h stat_job.h svr_resccost.h job_qs_upgrade.h queue_func.h req_locate.h \
                 req_runjob.h svr_chk_owner.h


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
		     dis_read.c geteusernam.c get_path_jobdata.c \
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
         display_alps_status.c login_nodes.c track_alps_reservations.cpp \
         batch_request.c user_info.c job_container.c exiting_jobs.c \
         receive_mom_communication.c process_mom_update.c execution_slot_tracker.cpp \
         job_usage_info.cpp incoming_request.c delete_all_tracker.cpp id_map.cpp \
         node_power_state.c req_modify_node.c mom_hierarchy_handler.cpp \
         completed_jobs_map.cpp restricted_host.cpp acl_special.cpp mail_throttler.cpp

install-exec-hook:
	$(PBS_MKDIRS) aux || :
	$(PBS_MKDIRS) server
	rm -f $(DESTDIR)$(sbindir)/$(program_prefix)qserverd$(program_suffix)$(EXEEXT)
	ln -s $(program_prefix)pbs_server$(program_suffix)$(EXEEXT) \
	   $(DESTDIR)$(sbindir)/$(program_prefix)qserverd$(program_suffix)$(EXEEXT)

uninstall-hook:
	rm -f $(DESTDIR)$(PBS_ENVIRON) $(DESTDIR)$(PBS_SERVER_HOME)/server_priv/nodes
	rm -f $(DESTDIR)$(sbindir)/$(program_prefix)qserverd$(program_suffix)$(EXEEXT)

