include $(top_srcdir)/buildutils/config.mk

CHECK_DIR = test

CLEANFILES = *.gcda *.gcno *.gcov

DIST_SUBDIRS=

include_HEADERS = array_func.h issue_request.h job_func.h node_func.h node_manager.h\
                  pbsd_main.h process_request.h queue_func.h queue_recov.h pbsd_init.h\
                  reply_send.h req_delete.h req_deletearray.h req_getcred.h\
                  req_holdjob.h req_jobobit.h req_locate.h req_manager.h req_message.h\
                  req_modify.h req_movejob.h req_quejob.h req_register.h req_rerun.h\
                  req_rescq.h req_runjob.h req_select.h req_shutdown.h req_signal.h\
                  req_stat.h req_track.h req_modify_node.h svr_connect.h svr_jobfunc.h\
                  queue_recycler.h svr_movejob.h svr_func.h ji_mutex.h job_route.h\
                  job_recov.h mom_hierarchy_handler.h completed_jobs_map.h

PBS_LIBS = ../lib/Libattr/libattr.a \
	   ../lib/Libsite/libsite.a \
		 ../lib/Libutils/libutils.a \
		 ../lib/Libifl/libifl.a \
	   ../lib/Libpbs/libtorque.la

sbin_PROGRAMS = pbs_server

pbs_server_LDADD = $(PBS_LIBS) $(ALPS_LIBS)
AM_CXXFLAGS = -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" -DPBS_ENVIRON=\"$(PBS_ENVIRON)\" `xml2-config --cflags`
AM_LIBS   =`xml2-config --libs`

pbs_server_SOURCES = accounting.cpp array_func.cpp array_upgrade.cpp attr_recov.cpp \
		     dis_read.cpp geteusernam.cpp get_path_jobdata.cpp \
		     issue_request.cpp job_attr_def.cpp job_func.cpp job_recov.cpp \
		     job_route.cpp node_attr_def.cpp node_func.cpp \
		     node_manager.cpp pbsd_init.cpp pbsd_main.cpp \
		     process_request.cpp queue_attr_def.cpp queue_func.cpp \
		     queue_recov.cpp reply_send.cpp req_delete.cpp \
		     req_deletearray.cpp req_getcred.cpp req_gpuctrl.cpp \
		     req_holdjob.cpp req_jobobit.cpp req_locate.cpp req_manager.cpp \
		     req_message.cpp req_modify.cpp req_movejob.cpp req_quejob.cpp \
		     req_register.cpp req_rerun.cpp req_rescq.cpp req_runjob.cpp \
		     req_select.cpp req_shutdown.cpp req_signal.cpp req_stat.cpp \
		     req_track.cpp resc_def_all.cpp run_sched.cpp stat_job.cpp \
		     svr_attr_def.cpp svr_chk_owner.cpp svr_connect.cpp \
		     svr_func.cpp svr_jobfunc.cpp svr_mail.cpp svr_movejob.cpp \
		     svr_recov.cpp svr_resccost.cpp svr_task.cpp req_tokens.cpp \
		     job_qs_upgrade.cpp req_holdarray.cpp svr_format_job.cpp \
			 job_recycler.cpp queue_recycler.cpp process_alps_status.cpp \
             display_alps_status.cpp login_nodes.cpp track_alps_reservations.cpp \
             batch_request.cpp user_info.cpp job_container.cpp exiting_jobs.cpp \
             receive_mom_communication.cpp process_mom_update.cpp execution_slot_tracker.cpp \
             job_usage_info.cpp incoming_request.cpp delete_all_tracker.cpp id_map.cpp \
             node_power_state.cpp req_modify_node.cpp mom_hierarchy_handler.cpp \
             completed_jobs_map.cpp

install-exec-hook:
	$(PBS_MKDIRS) aux || :
	$(PBS_MKDIRS) server
	rm -f $(DESTDIR)$(sbindir)/$(program_prefix)qserverd$(program_suffix)$(EXEEXT)
	ln -s $(program_prefix)pbs_server$(program_suffix)$(EXEEXT) \
	   $(DESTDIR)$(sbindir)/$(program_prefix)qserverd$(program_suffix)$(EXEEXT)

uninstall-hook:
	rm -f $(DESTDIR)$(PBS_ENVIRON) $(DESTDIR)$(PBS_SERVER_HOME)/server_priv/nodes
	rm -f $(DESTDIR)$(sbindir)/$(program_prefix)qserverd$(program_suffix)$(EXEEXT)

if HAVE_CHECK
check:
	$(MAKE) -C $(CHECK_DIR) $(MAKECMDGOALS)

.PHONY: cleancheck
cleancheck:
	cd test && $(MAKE) cleancheck
endif
