include $(top_srcdir)/buildutils/config.mk

if BUILD_LINUX_CGROUPS
NUMA_DIRS = allocation machine numa_chip numa_core numa_socket numa_pci_device numa_socket
endif

SERVER_UT_DIRS = accounting array_func array_upgrade attr_recov batch_request completed_jobs_map \
	delete_all_tracker dis_read display_alps_status execution_slot_tracker \
	exiting_jobs geteusernam get_path_jobdata id_map incoming_request \
	issue_request job_attr_def job_container job_func job_qs_upgrade job_recov \
	job_recycler job_usage_info login_nodes mom_hierarchy_handler node_func \
	node_manager pbsd_init pbsd_main process_alps_status process_mom_update \
	process_request queue_func queue_recov queue_recycler receive_mom_communication \
	reply_send req_delete req_deletearray req_getcred req_gpuctrl req_holdarray \
	req_holdjob req_jobobit req_locate req_manager req_message req_modify \
	req_movejob req_quejob req_register req_rerun req_rescq req_runjob req_select \
	req_shutdown req_signal req_stat req_tokens req_track resc_def_all run_sched \
	stat_job svr_chk_owner svr_connect svr_format_job svr_func svr_jobfunc svr_mail \
	svr_movejob svr_recov svr_resccost svr_task user_info acl_special restricted_host mail_throttler \
	job_array job

LIBUTILS_UT_DIRS = u_MXML u_groups u_hash_map_structs u_lock_ctl u_misc u_mom_hierarchy u_mu \
	u_mutex_mgr u_putenv u_threadpool u_tree u_users u_xml

LIBATTR_UT_DIRS = attr_atomic attr_fn_acl attr_fn_arst attr_fn_b attr_fn_c attr_fn_freq \
                  attr_fn_hold attr_fn_intr attr_fn_intr attr_fn_l attr_fn_ll attr_fn_nppcu \
                  attr_fn_resc attr_fn_size attr_fn_str attr_fn_time attr_fn_tokens attr_fn_tv \
                  attr_fn_unkn attr_func attr_node_func attr_fn_complete_req req complete_req \
                  attr_str_conversion

LIBCMDS_UT_DIRS = add_verify_resources ck_job_name cnt2server cvtdate get_server locate_job \
                  parse_at parse_depend parse_destid parse_equal parse_jobid parse_stage \
                  prepare_path prt_job_err set_attr set_resource

LIBCSV_UT_DIRS = csv

LIBDIS_UT_DIRS = discui_ discul_ disi10d_ disi10l_ disiui_ disp10d_ disp10l_ disrcs disrd disrf \
		disrfcs disrfst disrl disrl_ disrsc disrsi disrsi_ disrsl disrsl_ disrss disrst \
		disruc disrui disrul disrus diswcs diswf diswl_ diswsi diswsl diswui diswui_ diswul

LIBIFL_UT_DIRS = PBSD_gpuctrl2 PBSD_manage2 PBSD_manager_caps PBSD_msg2 PBSD_rdrpy PBSD_sig2 \
		PBSD_status PBSD_status2 PBSD_submit_caps PBS_attr dec_Authen dec_CpyFil dec_Gpu \
		dec_JobCred dec_JobFile dec_JobId dec_JobObit dec_Manage dec_MoveJob dec_MsgJob \
		dec_QueueJob dec_Reg dec_ReqExt dec_ReqHdr dec_Resc dec_ReturnFile dec_RunJob \
		dec_Shut dec_Sig dec_Status dec_Track dec_attrl dec_attropl dec_rpyc dec_rpys \
		dec_svrattrl enc_CpyFil enc_Gpu enc_JobCred enc_JobFile enc_JobId enc_JobObit \
		enc_Manage enc_MoveJob enc_MsgJob enc_QueueJob enc_QueueJob_hash enc_Reg \
		enc_ReqExt enc_ReqHdr enc_ReturnFile enc_RunJob enc_Shut enc_Sig enc_Status \
		enc_Track enc_attrl enc_attropl enc_attropl_hash enc_reply enc_svrattrl \
		get_svrport list_link nonblock pbsD_alterjo pbsD_asyrun pbsD_chkptjob \
		pbsD_connect pbsD_deljob pbsD_gpuctrl pbsD_holdjob pbsD_locjob pbsD_manager \
		pbsD_movejob pbsD_msgjob pbsD_orderjo pbsD_rerunjo pbsD_resc pbsD_rlsjob \
		pbsD_runjob pbsD_selectj pbsD_sigjob pbsD_stagein pbsD_statjob pbsD_statnode \
		pbsD_statque pbsD_statsrv pbsD_submit pbsD_submit_hash pbsD_termin pbs_geterrmg \
		pbs_statfree tcp_dis tm torquecfg trq_auth

LIBLOG_UT_DIRS = chk_file_sec log_event pbs_log pbs_messages setup_env

LIBNET_UT_DIRS = conn_table get_hostaddr get_hostname md5 net_client net_common net_server \
		net_set_clse port_forwarding rm server_core net_cache

LIBSITE_UT_DIRS = site_allow_u site_alt_rte site_check_u site_map_usr site_mom_chu site_mom_ckp \
		site_mom_jst

CMDS_UT_DIRS = MXML common_cmds pbs_track pbsdsh pbsnodes pbspd pbspoe qalter qchkpt qdel qdisable \
               qenable qgpumode qgpureset qhold qmgr qmove qmsg qorder qrerun qrls qrun qselect \
	       qsig qstart qstat qstop qsub_functions qterm

MISC_UT_DIRS = momctl

MOM_UT_DIRS = alps_reservations catch_child checkpoint cray_energy generate_alps_status \
	mom_comm mom_inter mom_job_func mom_mach mom_main mom_process_request mom_req_quejob \
	mom_server mom_start parse_config pbs_demux prolog release_reservation requests \
	start_exec tmsock_recov
if BUILDCPA
  MOM_UT_DIRS += cray_cpa
endif

if BUILDPMIX
  MOM_UT_DIRS += pmix_interface pmix_operation pmix_tracker
endif

if INCLUDE_PAM
PAM_DIRS = pam
endif

TRQAUTH_DIRS = trq_auth_daemon

CHECK_LIBS = scaffold_fail torque_test_lib 

CHECK_DIRS = ${SERVER_UT_DIRS} ${LIBUTILS_UT_DIRS} \
						 ${LIBATTR_UT_DIRS} ${LIBCMDS_UT_DIRS} ${LIBDIS_UT_DIRS} ${LIBCSV_UT_DIRS} \
						 ${LIBIFL_UT_DIRS} ${LIBLOG_UT_DIRS} ${CMDS_UT_DIRS} ${MISC_UT_DIRS} ${NUMA_DIRS} \
						 ${MOM_UT_DIRS} ${PAM_DIRS} ${TRQAUTH_DIRS}

$(CHECK_LIBS)::
	$(MAKE) -C $@ $(MAKECMDGOALS)

FORCE:

$(CHECK_DIRS):: $(CHECK_LIBS) FORCE
	$(MAKE) -C $@ $(MAKECMDGOALS)

check: $(CHECK_DIRS)

cleancheck:
	@for dir in $(CHECK_DIRS); do (cd $$dir && $(MAKE) clean); done

clean-local: $(CHECK_DIRS)
