include $(top_srcdir)/buildutils/config.mk

CHECK_DIR = test

CPPFLAGS = @CPPFLAGS@ -DPBS_DEFAULT_FILE=\"$(PBS_DEFAULT_FILE)\" -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\"
DIST_SUBDIRS =
CLEANFILES = *.gcda *.gcno *.gcov

include_HEADERS = lib_ifl.h

noinst_LIBRARIES = libifl.a

libifl_a_SOURCES = PBSD_gpuctrl2.cpp PBSD_manage2.cpp PBSD_manager_caps.cpp\
                   PBSD_msg2.cpp PBSD_rdrpy.cpp PBSD_sig2.cpp PBSD_status.cpp\
                   PBSD_status2.cpp PBSD_submit_caps.cpp PBS_attr.cpp PBS_data.cpp\
                   dec_Authen.cpp dec_CpyFil.cpp dec_Gpu.cpp dec_JobCred.cpp dec_JobFile.cpp\
                   dec_JobId.cpp dec_JobObit.cpp dec_Manage.cpp dec_MoveJob.cpp dec_MsgJob.cpp\
                   dec_QueueJob.cpp dec_Reg.cpp dec_ReqExt.cpp dec_ReqHdr.cpp dec_Resc.cpp\
                   dec_ReturnFile.cpp dec_RunJob.cpp dec_Shut.cpp dec_Sig.cpp dec_Status.cpp\
                   dec_Track.cpp dec_attrl.cpp dec_attropl.cpp dec_rpyc.cpp dec_rpys.cpp\
                   dec_svrattrl.cpp enc_CpyFil.cpp enc_Gpu.cpp enc_JobCred.cpp enc_JobFile.cpp\
                   enc_JobId.cpp enc_JobObit.cpp enc_Manage.cpp enc_MoveJob.cpp enc_MsgJob.cpp\
                   enc_QueueJob.cpp enc_QueueJob_hash.cpp enc_Reg.cpp enc_ReqExt.cpp\
                   enc_ReqHdr.cpp enc_ReturnFile.cpp enc_RunJob.cpp enc_Shut.cpp enc_Sig.cpp\
                   enc_Status.cpp enc_Track.cpp enc_attrl.cpp enc_attropl.cpp\
                   enc_attropl_hash.cpp enc_reply.cpp enc_svrattrl.cpp get_svrport.cpp\
                   list_link.cpp nonblock.cpp pbsD_alterjo.cpp pbsD_asyrun.cpp pbsD_chkptjob.cpp\
                   pbsD_connect.cpp pbsD_deljob.cpp pbsD_gpuctrl.cpp pbsD_holdjob.cpp\
                   pbsD_locjob.cpp pbsD_manager.cpp pbsD_movejob.cpp pbsD_msgjob.cpp\
                   pbsD_orderjo.cpp pbsD_rerunjo.cpp pbsD_resc.cpp pbsD_rlsjob.cpp\
                   pbsD_runjob.cpp pbsD_selectj.cpp pbsD_sigjob.cpp pbsD_stagein.cpp\
                   pbsD_statjob.cpp pbsD_statnode.cpp pbsD_statque.cpp pbsD_statsrv.cpp\
                   pbsD_submit.cpp pbsD_submit_hash.cpp pbsD_termin.cpp pbs_geterrmg.cpp\
                   pbs_statfree.cpp tcp_dis.cpp tm.cpp torquecfg.cpp trq_auth.cpp\
                   enc_PowerState.cpp dec_PowerState.cpp

if HAVE_CHECK
check:
	$(MAKE) -C $(CHECK_DIR) $(MAKECMDGOALS)

PHONY: cleancheck
cleancheck:
	cd test && $(MAKE) cleancheck
endif
