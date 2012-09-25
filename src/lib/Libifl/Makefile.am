include $(top_srcdir)/buildutils/config.mk

if HAVE_CHECK
SUBDIRS = test
endif

CPPFLAGS = -DPBS_DEFAULT_FILE=\"$(PBS_DEFAULT_FILE)\" -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\"
DIST_SUBDIRS =
CLEANFILES = *.gcda *.gcno *.gcov

include_HEADERS = lib_ifl.h

noinst_LIBRARIES = libifl.a

libifl_a_SOURCES = PBSD_gpuctrl2.c PBSD_manage2.c PBSD_manager_caps.c PBSD_msg2.c PBSD_rdrpy.c PBSD_sig2.c PBSD_status.c PBSD_status2.c PBSD_submit_caps.c PBS_attr.c PBS_data.c dec_Authen.c dec_CpyFil.c dec_Gpu.c dec_JobCred.c dec_JobFile.c dec_JobId.c dec_JobObit.c dec_Manage.c dec_MoveJob.c dec_MsgJob.c dec_QueueJob.c dec_Reg.c dec_ReqExt.c dec_ReqHdr.c dec_Resc.c dec_ReturnFile.c dec_RunJob.c dec_Shut.c dec_Sig.c dec_Status.c dec_Track.c dec_attrl.c dec_attropl.c dec_rpyc.c dec_rpys.c dec_svrattrl.c enc_CpyFil.c enc_Gpu.c enc_JobCred.c enc_JobFile.c enc_JobId.c enc_JobObit.c enc_Manage.c enc_MoveJob.c enc_MsgJob.c enc_QueueJob.c enc_QueueJob_hash.c enc_Reg.c enc_ReqExt.c enc_ReqHdr.c enc_ReturnFile.c enc_RunJob.c enc_Shut.c enc_Sig.c enc_Status.c enc_Track.c enc_attrl.c enc_attropl.c enc_attropl_hash.c enc_reply.c enc_svrattrl.c get_svrport.c list_link.c nonblock.c pbsD_alterjo.c pbsD_asyrun.c pbsD_chkptjob.c pbsD_connect.c pbsD_deljob.c pbsD_gpuctrl.c pbsD_holdjob.c pbsD_locjob.c pbsD_manager.c pbsD_movejob.c pbsD_msgjob.c pbsD_orderjo.c pbsD_rerunjo.c pbsD_resc.c pbsD_rlsjob.c pbsD_runjob.c pbsD_selectj.c pbsD_sigjob.c pbsD_stagein.c pbsD_statjob.c pbsD_statnode.c pbsD_statque.c pbsD_statsrv.c pbsD_submit.c pbsD_submit_hash.c pbsD_termin.c pbs_geterrmg.c pbs_statfree.c rpp.c tcp_dis.c tm.c torquecfg.c trq_auth.c
