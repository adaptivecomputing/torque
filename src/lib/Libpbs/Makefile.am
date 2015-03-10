include $(top_srcdir)/buildutils/config.mk

AM_CXXFLAGS = -I$(top_srcdir)/src/lib/Libdis -DIFF_PATH=\"$(IFF_PATH)\" -DPBS_DEFAULT_FILE=\"$(PBS_DEFAULT_FILE)\" -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\"

CLEANFILES = *.gcda *.gcno *.gcov

lib_LTLIBRARIES = libtorque.la

libtorque_la_LDFLAGS = -version-info 2:0:0

libtorque_la_SOURCES = ../Libcsv/csv.cpp ../Libdis/dis.cpp \
        ../Libdis/discui_.cpp ../Libdis/discul_.cpp \
		    ../Libdis/disi10d_.cpp ../Libdis/disi10l_.cpp \
		    ../Libdis/disiui_.cpp ../Libdis/disp10d_.cpp \
		    ../Libdis/disp10l_.cpp ../Libdis/disrcs.cpp \
		    ../Libdis/disrd.cpp ../Libdis/disrf.cpp ../Libdis/disrfcs.cpp \
		    ../Libdis/disrfst.cpp ../Libdis/disrl_.cpp ../Libdis/disrl.cpp \
		    ../Libdis/disrsc.cpp ../Libdis/disrsi_.cpp \
		    ../Libdis/disrsi.cpp ../Libdis/disrsl_.cpp \
		    ../Libdis/disrsl.cpp ../Libdis/disrss.cpp ../Libdis/disrst.cpp \
		    ../Libdis/disruc.cpp ../Libdis/disrui.cpp ../Libdis/disrul.cpp \
		    ../Libdis/disrus.cpp ../Libdis/diswcs.cpp ../Libdis/diswf.cpp \
		    ../Libdis/diswl_.cpp ../Libdis/diswsi.cpp ../Libdis/diswsl.cpp \
		    ../Libdis/diswui_.cpp ../Libdis/diswui.cpp \
		    ../Libdis/diswul.cpp \
		    ../Libutils/u_mutex_mgr.cpp \
		    ../Libifl/dec_attrl.cpp ../Libifl/dec_attropl.cpp \
		    ../Libifl/dec_Authen.cpp ../Libifl/dec_CpyFil.cpp \
		    ../Libifl/dec_JobCred.cpp ../Libifl/dec_JobFile.cpp \
		    ../Libifl/dec_JobId.cpp ../Libifl/dec_JobObit.cpp \
		    ../Libifl/dec_Manage.cpp ../Libifl/dec_MoveJob.cpp \
		    ../Libifl/dec_MsgJob.cpp ../Libifl/dec_QueueJob.cpp \
		    ../Libifl/dec_Reg.cpp ../Libifl/dec_ReqExt.cpp \
		    ../Libifl/dec_ReqHdr.cpp ../Libifl/dec_Resc.cpp \
		    ../Libifl/dec_ReturnFile.cpp \
		    ../Libifl/dec_rpyc.cpp ../Libifl/dec_rpys.cpp \
		    ../Libifl/dec_RunJob.cpp ../Libifl/dec_Shut.cpp \
		    ../Libifl/dec_Sig.cpp ../Libifl/dec_Status.cpp \
		    ../Libifl/dec_svrattrl.cpp ../Libifl/dec_Track.cpp \
		    ../Libifl/dec_Gpu.cpp \
		    ../Libifl/enc_attrl.cpp ../Libifl/enc_attropl.cpp \
				../Libifl/enc_attropl_hash.cpp \
		    ../Libifl/enc_CpyFil.cpp ../Libifl/enc_JobCred.cpp \
		    ../Libifl/enc_JobFile.cpp ../Libifl/enc_JobId.cpp \
		    ../Libifl/enc_JobObit.cpp ../Libifl/enc_Manage.cpp \
		    ../Libifl/enc_MoveJob.cpp ../Libifl/enc_MsgJob.cpp \
		    ../Libifl/enc_QueueJob.cpp ../Libifl/enc_Reg.cpp \
				../Libifl/enc_QueueJob_hash.cpp \
		    ../Libifl/enc_reply.cpp ../Libifl/enc_ReturnFile.cpp \
		    ../Libifl/enc_ReqExt.cpp ../Libifl/enc_Gpu.cpp \
		    ../Libifl/enc_ReqHdr.cpp ../Libifl/enc_RunJob.cpp \
		    ../Libifl/enc_Shut.cpp ../Libifl/enc_Sig.cpp \
		    ../Libifl/enc_Status.cpp ../Libifl/enc_svrattrl.cpp \
		    ../Libifl/enc_Track.cpp ../Libifl/get_svrport.cpp \
             ../Libifl/enc_PowerState.cpp ../Libifl/dec_PowerState.cpp \
		    ../Libifl/nonblock.cpp ../Libifl/PBS_attr.cpp \
		    ../Libifl/pbsD_alterjo.cpp ../Libifl/pbsD_asyrun.cpp \
		    ../Libifl/PBS_data.cpp ../Libifl/pbsD_connect.cpp \
		    ../Libifl/pbsD_deljob.cpp ../Libifl/pbsD_holdjob.cpp  \
		    ../Libifl/pbsD_chkptjob.cpp  ../Libifl/pbsD_locjob.cpp \
		    ../Libifl/pbsD_gpuctrl.cpp ../Libifl/PBSD_gpuctrl2.cpp \
		    ../Libifl/PBSD_manage2.cpp ../Libifl/pbsD_manager.cpp \
		    ../Libifl/pbsD_movejob.cpp ../Libifl/PBSD_manager_caps.cpp \
		    ../Libifl/PBSD_msg2.cpp ../Libifl/pbsD_msgjob.cpp \
		    ../Libifl/pbsD_orderjo.cpp ../Libifl/PBSD_rdrpy.cpp \
		    ../Libifl/pbsD_rerunjo.cpp ../Libifl/pbsD_resc.cpp \
		    ../Libifl/pbsD_rlsjob.cpp ../Libifl/pbsD_runjob.cpp \
		    ../Libifl/pbsD_selectj.cpp ../Libifl/PBSD_sig2.cpp \
		    ../Libifl/pbsD_sigjob.cpp ../Libifl/pbsD_stagein.cpp \
		    ../Libifl/pbsD_statjob.cpp ../Libifl/pbsD_statnode.cpp \
		    ../Libifl/pbsD_statque.cpp ../Libifl/pbsD_statsrv.cpp \
		    ../Libifl/PBSD_status2.cpp ../Libifl/PBSD_status.cpp \
		    ../Libifl/pbsD_submit.cpp  ../Libifl/PBSD_submit_caps.cpp \
		    ../Libifl/pbsD_submit_hash.cpp  \
		    ../Libifl/pbsD_termin.cpp ../Libifl/pbs_geterrmg.cpp \
		    ../Libifl/pbs_statfree.cpp \
		    ../Libifl/tcp_dis.cpp ../Libifl/tm.cpp ../Libifl/list_link.cpp \
                    ../Libifl/trq_auth.cpp  ../Libifl/torquecfg.cpp \
		    ../Libcmds/ck_job_name.cpp ../Libcmds/cnt2server.cpp \
		    ../Libcmds/cvtdate.cpp ../Libcmds/get_server.cpp \
                    ../Libcmds/locate_job.cpp ../Libcmds/parse_at.cpp \
		    ../Libcmds/parse_depend.cpp ../Libcmds/parse_destid.cpp \
                    ../Libcmds/parse_equal.cpp ../Libcmds/parse_jobid.cpp \
		    ../Libcmds/parse_stage.cpp \
                    ../Libcmds/prepare_path.cpp ../Libcmds/prt_job_err.cpp \
		    ../Libcmds/set_attr.cpp ../Libcmds/set_resource.cpp \
                    ../Libcmds/add_verify_resources.cpp \
		    ../Liblog/chk_file_sec.cpp ../Liblog/log_event.cpp \
		    ../Liblog/pbs_log.cpp ../Liblog/pbs_messages.cpp \
                    ../Liblog/setup_env.cpp ../Libnet/conn_table.cpp \
		    ../Libnet/get_hostaddr.cpp ../Libnet/get_hostname.cpp \
		    ../Libnet/md5.cpp ../Libnet/net_common.cpp ../Libnet/net_client.cpp \
                    ../Libnet/net_server.cpp ../Libnet/net_set_clse.cpp \
                    ../Libnet/server_core.cpp \
		    ../Libnet/rm.cpp ../Libnet/port_forwarding.cpp \
                    ../Libnet/net_cache.cpp ../Libutils/u_lock_ctl.cpp \
                    ../Libutils/u_hash_map_structs.cpp \
                    ../Libutils/u_threadpool.cpp ../Libutils/u_users.cpp ../Libutils/u_wrapper.cpp



