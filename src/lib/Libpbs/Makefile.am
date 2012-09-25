include $(top_srcdir)/buildutils/config.mk

AM_CPPFLAGS += -I$(top_srcdir)/src/lib/Libdis -DIFF_PATH=\"$(IFF_PATH)\" -DPBS_DEFAULT_FILE=\"$(PBS_DEFAULT_FILE)\" -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\"

CLEANFILES = *.gcda *.gcno *.gcov

lib_LTLIBRARIES = libtorque.la

libtorque_la_LDFLAGS = -version-info 2:0:0

libtorque_la_SOURCES = ../Libcsv/csv.c ../Libdis/dis.c \
        ../Libdis/discui_.c ../Libdis/discul_.c \
		    ../Libdis/disi10d_.c ../Libdis/disi10l_.c \
		    ../Libdis/disiui_.c ../Libdis/disp10d_.c \
		    ../Libdis/disp10l_.c ../Libdis/disrcs.c \
		    ../Libdis/disrd.c ../Libdis/disrf.c ../Libdis/disrfcs.c \
		    ../Libdis/disrfst.c ../Libdis/disrl_.c ../Libdis/disrl.c \
		    ../Libdis/disrsc.c ../Libdis/disrsi_.c \
		    ../Libdis/disrsi.c ../Libdis/disrsl_.c \
		    ../Libdis/disrsl.c ../Libdis/disrss.c ../Libdis/disrst.c \
		    ../Libdis/disruc.c ../Libdis/disrui.c ../Libdis/disrul.c \
		    ../Libdis/disrus.c ../Libdis/diswcs.c ../Libdis/diswf.c \
		    ../Libdis/diswl_.c ../Libdis/diswsi.c ../Libdis/diswsl.c \
		    ../Libdis/diswui_.c ../Libdis/diswui.c \
		    ../Libdis/diswul.c \
		    ../Libifl/dec_attrl.c ../Libifl/dec_attropl.c \
		    ../Libifl/dec_Authen.c ../Libifl/dec_CpyFil.c \
		    ../Libifl/dec_JobCred.c ../Libifl/dec_JobFile.c \
		    ../Libifl/dec_JobId.c ../Libifl/dec_JobObit.c \
		    ../Libifl/dec_Manage.c ../Libifl/dec_MoveJob.c \
		    ../Libifl/dec_MsgJob.c ../Libifl/dec_QueueJob.c \
		    ../Libifl/dec_Reg.c ../Libifl/dec_ReqExt.c \
		    ../Libifl/dec_ReqHdr.c ../Libifl/dec_Resc.c \
		    ../Libifl/dec_ReturnFile.c \
		    ../Libifl/dec_rpyc.c ../Libifl/dec_rpys.c \
		    ../Libifl/dec_RunJob.c ../Libifl/dec_Shut.c \
		    ../Libifl/dec_Sig.c ../Libifl/dec_Status.c \
		    ../Libifl/dec_svrattrl.c ../Libifl/dec_Track.c \
		    ../Libifl/dec_Gpu.c \
		    ../Libifl/enc_attrl.c ../Libifl/enc_attropl.c \
				../Libifl/enc_attropl_hash.c \
		    ../Libifl/enc_CpyFil.c ../Libifl/enc_JobCred.c \
		    ../Libifl/enc_JobFile.c ../Libifl/enc_JobId.c \
		    ../Libifl/enc_JobObit.c ../Libifl/enc_Manage.c \
		    ../Libifl/enc_MoveJob.c ../Libifl/enc_MsgJob.c \
		    ../Libifl/enc_QueueJob.c ../Libifl/enc_Reg.c \
				../Libifl/enc_QueueJob_hash.c \
		    ../Libifl/enc_reply.c ../Libifl/enc_ReturnFile.c \
		    ../Libifl/enc_ReqExt.c ../Libifl/enc_Gpu.c \
		    ../Libifl/enc_ReqHdr.c ../Libifl/enc_RunJob.c \
		    ../Libifl/enc_Shut.c ../Libifl/enc_Sig.c \
		    ../Libifl/enc_Status.c ../Libifl/enc_svrattrl.c \
		    ../Libifl/enc_Track.c ../Libifl/get_svrport.c \
		    ../Libifl/nonblock.c ../Libifl/PBS_attr.c \
		    ../Libifl/pbsD_alterjo.c ../Libifl/pbsD_asyrun.c \
		    ../Libifl/PBS_data.c ../Libifl/pbsD_connect.c \
		    ../Libifl/pbsD_deljob.c ../Libifl/pbsD_holdjob.c  \
		    ../Libifl/pbsD_chkptjob.c  ../Libifl/pbsD_locjob.c \
		    ../Libifl/pbsD_gpuctrl.c ../Libifl/PBSD_gpuctrl2.c \
		    ../Libifl/PBSD_manage2.c ../Libifl/pbsD_manager.c \
		    ../Libifl/pbsD_movejob.c ../Libifl/PBSD_manager_caps.c \
		    ../Libifl/PBSD_msg2.c ../Libifl/pbsD_msgjob.c \
		    ../Libifl/pbsD_orderjo.c ../Libifl/PBSD_rdrpy.c \
		    ../Libifl/pbsD_rerunjo.c ../Libifl/pbsD_resc.c \
		    ../Libifl/pbsD_rlsjob.c ../Libifl/pbsD_runjob.c \
		    ../Libifl/pbsD_selectj.c ../Libifl/PBSD_sig2.c \
		    ../Libifl/pbsD_sigjob.c ../Libifl/pbsD_stagein.c \
		    ../Libifl/pbsD_statjob.c ../Libifl/pbsD_statnode.c \
		    ../Libifl/pbsD_statque.c ../Libifl/pbsD_statsrv.c \
		    ../Libifl/PBSD_status2.c ../Libifl/PBSD_status.c \
		    ../Libifl/pbsD_submit.c  ../Libifl/PBSD_submit_caps.c \
		    ../Libifl/pbsD_submit_hash.c  \
		    ../Libifl/pbsD_termin.c ../Libifl/pbs_geterrmg.c \
		    ../Libifl/pbs_statfree.c ../Libifl/rpp.c \
		    ../Libifl/tcp_dis.c ../Libifl/tm.c ../Libifl/list_link.c \
				../Libifl/trq_auth.c  ../Libifl/torquecfg.c \
		    ../Libcmds/ck_job_name.c ../Libcmds/cnt2server.c \
		    ../Libcmds/cvtdate.c ../Libcmds/get_server.c \
        ../Libcmds/locate_job.c ../Libcmds/parse_at.c \
		    ../Libcmds/parse_depend.c ../Libcmds/parse_destid.c \
        ../Libcmds/parse_equal.c ../Libcmds/parse_jobid.c \
		    ../Libcmds/parse_stage.c \
        ../Libcmds/prepare_path.c ../Libcmds/prt_job_err.c \
		    ../Libcmds/set_attr.c ../Libcmds/set_resource.c \
				../Libcmds/add_verify_resources.c \
		    ../Liblog/chk_file_sec.c ../Liblog/log_event.c \
		    ../Liblog/pbs_log.c ../Liblog/pbs_messages.c \
        ../Liblog/setup_env.c ../Libnet/conn_table.c \
		    ../Libnet/get_hostaddr.c ../Libnet/get_hostname.c \
		    ../Libnet/md5.c ../Libnet/net_common.c ../Libnet/net_client.c \
        ../Libnet/net_server.c ../Libnet/net_set_clse.c \
				../Libnet/server_core.c \
		    ../Libnet/rm.c ../Libnet/port_forwarding.c \
				../Libnet/net_cache.c ../Libutils/u_lock_ctl.c \
				../Libutils/u_hash_map_structs.c ../Libutils/u_memmgr.c \
				../Libutils/u_hash_table.c ../Libutils/u_resizable_array.c \
				../Libutils/u_threadpool.c



