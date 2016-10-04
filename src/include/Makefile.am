include $(top_srcdir)/buildutils/config.mk

include_HEADERS = pbs_error.h pbs_error_db.h pbs_ifl.h tm.h tm_.h rpp.h rm.h license_pbs.h tcp.h \
									log.h trq_plugin_api.h

noinst_HEADERS = acct.h array.h assertions.h attribute.h		\
		 batch_request.h cmds.h credential.h csv.h dis.h	\
		 dis_init.h get_path_jobdata.h libcmds.h libpbs.h list_link.h 	\
		 mcom.h md5.h mom_func.h mom_job_cleanup.h		\
		 net_connect.h pbs_assert.h pbs_nodes.h pbs_proto.h	\
		 pbs_constants.h pbs_cmds.h portability.h qmgr.h qmgr_node_public.h		\
		 qmgr_node_readonly.h qmgr_que_public.h			\
		 qmgr_que_readonly.h qmgr_svr_public.h			\
		 qmgr_svr_readonly.h queue.h resmon.h resource.h	\
		 sched_cmds.h server.h server_limits.h svrfunc.h	\
		 tracking.h work_task.h port_forwarding.h pbs_cpa.h	\
		 pbs_cpuset.h pbs_batchreqtype_db.h utils.h u_tree.h	\
		 threadpool.h u_hash_map_structs.h uthash.h pbs_job.h  		\
		 mom_hierarchy.h mom_server.h		\
		 alps_constants.h alps_functions.h login_nodes.h	\
		 track_alps_reservations.hpp net_cache.h user_info.h	\
		 exiting_jobs.h mom_update.h dis_internal.h             \
		 mutex_mgr.hpp mom_memory.h execution_slot_tracker.hpp  \
		 mom_config.h node_internals.hpp numa_node.hpp server_comm.h \
		 delete_all_tracker.hpp timer.hpp id_map.hpp container.hpp \
		 node_frequency.hpp cpu_frequency.hpp sys_file.hpp power_state.hpp \
		 job_usage_info.hpp machine.hpp req.hpp complete_req.hpp trq_cgroups.h \
		 job_recovery.h allocation.hpp attr_req_info.hpp acl_special.hpp restricted_host.hpp \
		 pbs_helper.h mail_throttler.hpp lib_ifl.h runjob_help.hpp pmix_tracker.hpp \
		 pmix_operation.hpp job_host_data.hpp policy_values.h plugin_internal.h json/json.h \
		 json/json-forwards.h

BUILT_SOURCES = site_job_attr_def.h site_job_attr_enum.h \
		site_qmgr_node_print.h site_qmgr_que_print.h \
		site_qmgr_svr_print.h site_que_attr_def.h \
		site_que_attr_enum.h site_svr_attr_def.h \
		site_svr_attr_enum.h

EXTRA_DIST = site_job_attr_def.ht site_job_attr_enum.ht \
	     site_qmgr_node_print.ht site_qmgr_que_print.ht \
	     site_qmgr_svr_print.ht site_que_attr_def.ht \
	     site_que_attr_enum.ht site_svr_attr_def.ht \
	     site_svr_attr_enum.ht

CLEANFILES = $(BUILT_SOURCES)

SUFFIXES = .ht .h

.ht.h:
	cp $< $@

dist-hook:
	(cd $(distdir) && rm -f $(BUILT_SOURCES))
