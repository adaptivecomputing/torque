include $(top_srcdir)/buildutils/config.mk

include_HEADERS = libcmds.h license_pbs.h pbs_constants.h pbs_cmds.h pbs_error.h \
									pbs_error_db.h pbs_ifl.h rm.h rpp.h tm.h tm_.h u_hash_map_structs.h \
									u_memmgr.h uthash.h tcp.h

noinst_HEADERS = acct.h array.h assertions.h attribute.h batch_request.h \
		 cmds.h credential.h csv.h \
		 dis.h dis_init.h pbs_job.h libpbs.h list_link.h log.h \
		 mcom.h md5.h mom_func.h net_connect.h pbs_assert.h \
		 pbs_nodes.h pbs_proto.h portability.h qmgr.h \
		 qmgr_node_public.h qmgr_node_readonly.h qmgr_que_public.h \
		 qmgr_que_readonly.h qmgr_svr_public.h qmgr_svr_readonly.h \
		 queue.h resmon.h resource.h sched_cmds.h server.h \
		 server_limits.h svrfunc.h tracking.h work_task.h \
		 port_forwarding.h pbs_cpa.h pbs_cpuset.h  \
		 pbs_batchreqtype_db.h utils.h u_tree.h threadpool.h \
		 resizable_array.h hash_table.h mom_hierarchy.h \
		 dynamic_string.h mom_server.h alps_constants.h \
		 alps_functions.h login_nodes.h track_alps_reservations.h \
		 net_cache.h user_info.h hash_map.h exiting_jobs.h

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
