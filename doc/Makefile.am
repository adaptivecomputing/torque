include $(top_srcdir)/buildutils/config.mk

noinst_PROGRAMS = soelim
soelim_SOURCES = soelim.c

# ensures soelim is built early so that parallel makes work
BUILT_SOURCES = soelim

SUBDIRS = man1 man3 man7 man8

EXTRA_DIST = man1/qalter.1.in man1/qdel.1.in man1/qhold.1.in man1/qmove.1.in \
	     man1/qmsg.1.in man1/qorder.1.in man1/qrerun.1.in man1/qrls.1.in \
	     man1/qselect.1.in man1/qsig.1.in man1/qstat.1.in man1/qsub.1.in \
	     man1/pbs.1.in man1/pbsdsh.1.in man1/nqs2pbs.1.in man1/basl2c.1.in \
	     man1/xpbs.1.in man1/xpbsmon.1.in man1/qmgr.1.in man1/qchkpt.1.in \
	     man1/qgpumode.1.in man1/qgpureset.1.in \
	     \
	     man3/pbs_alterjob.3.in man3/pbs_checkpointjob.3.in \
	     man3/pbs_connect.3.in man3/pbs_gpumode.3.in man3/pbs_gpureset.3.in \
	     man3/pbs_default.3.in man3/pbs_deljob.3.in man3/pbs_disconnect.3.in \
	     man3/pbs_fbserver.3.in man3/pbs_geterrmsg.3.in \
	     man3/pbs_get_server_list.3.in man3/pbs_holdjob.3.in \
	     man3/pbs_locate.3.in man3/pbs_manager.3.in \
	     man3/pbs_movejob.3.in man3/pbs_msgjob.3.in man3/pbs_orderjob.3.in \
	     man3/pbs_rerunjob.3.in man3/pbs_rescquery.3.in \
	     man3/pbs_rescreserve.3.in man3/pbs_rlsjob.3.in man3/pbs_runjob.3.in \
	     man3/pbs_selectjob.3.in man3/pbs_selstat.3.in man3/pbs_sigjob.3.in \
	     man3/pbs_stagein.3.in man3/pbs_statjob.3.in man3/pbs_statnode.3.in \
	     man3/pbs_statque.3.in man3/pbs_statserver.3.in man3/pbs_submit.3.in \
	     man3/pbs_terminate.3.in \
	     \
	     man7/pbs_job_attributes.7.in man7/pbs_queue_attributes.7.in \
	     man7/pbs_server_attributes.7.in \
	     man7/pbs_resources_aix4.7.in man7/pbs_resources_digitalunix.7.in \
	     man7/pbs_resources_aix5.7.in man7/pbs_resources_darwin.7.in \
	     man7/pbs_resources_freebsd.7.in man7/pbs_resources_freebsd5.7.in \
	     man7/pbs_resources_fujitsu.7.in \
	     man7/pbs_resources_hpux10.7.in man7/pbs_resources_irix5.7.in \
	     man7/pbs_resources_hpux11.7.in \
	     man7/pbs_resources_irix6.7.in man7/pbs_resources_irix6array.7.in \
	     man7/pbs_resources_linux.7.in man7/pbs_resources_netbsd.7.in \
	     man7/pbs_resources_solaris5.7.in man7/pbs_resources_solaris7.7.in \
	     man7/pbs_resources_sp2.7.in man7/pbs_resources_sunos4.7.in \
	     man7/pbs_resources_unicos8.7.in man7/pbs_resources_unicosmk2.7.in \
	     \
	     man8/pbs_mom.8.in man8/pbsnodes.8.in \
	     man8/pbs_sched_basl.8.in man8/pbs_sched_cc.8.in \
	     man8/pbs_sched_tcl.8.in man8/pbs_server.8.in \
	     man8/qdisable.8.in man8/qenable.8.in \
	     man8/qrun.8.in man8/qstart.8.in man8/qstop.8.in \
	     man8/qterm.8.in \
	     \
	     READ_ME admin_guide.ps v2_2_ers.pdf v2_2_ids.pdf doc_fonts \
	     ers/ers.macros ers/ers_setup.ms ers/pbs_resources_all.so \
	     man1/Makefile.am man1/Makefile.in \
	     man3/Makefile.am man3/Makefile.in \
	     man8/Makefile.am man8/Makefile.in

nodist_man1_MANS = man1/qalter.1 man1/qdel.1 man1/qhold.1 man1/qmove.1 \
		   man1/qmsg.1 man1/qorder.1 man1/qrerun.1 man1/qrls.1 \
		   man1/qselect.1 man1/qsig.1 man1/qstat.1 man1/qsub.1 \
		   man1/pbs.1 man1/pbsdsh.1 man1/nqs2pbs.1 man1/basl2c.1 \
		   man1/xpbs.1 man1/xpbsmon.1 man1/qmgr.1 man1/qchkpt.1 \
		   man1/qgpumode.1 man1/qgpureset.1

nodist_man3_MANS = man3/pbs_alterjob.3 \
			    man3/pbs_checkpointjob.3 \
			    man3/pbs_connect.3 \
			    man3/pbs_default.3 \
	 		    man3/pbs_deljob.3 \
			    man3/pbs_disconnect.3 \
	 		    man3/pbs_fbserver.3 \
	 		    man3/pbs_geterrmsg.3 \
	 		    man3/pbs_get_server_list.3 \
          man3/pbs_gpumode.3 \
          man3/pbs_gpureset.3 \
			    man3/pbs_holdjob.3 \
			    man3/pbs_locate.3 \
	 		    man3/pbs_manager.3 \
			    man3/pbs_movejob.3 \
			    man3/pbs_msgjob.3 \
	 		    man3/pbs_orderjob.3 \
			    man3/pbs_rerunjob.3 \
	 		    man3/pbs_rescquery.3 \
			    man3/pbs_rescreserve.3 \
	 		    man3/pbs_rlsjob.3 \
			    man3/pbs_runjob.3 \
			    man3/pbs_selectjob.3 \
	 		    man3/pbs_selstat.3 \
			    man3/pbs_sigjob.3 \
			    man3/pbs_stagein.3 \
	 		    man3/pbs_statjob.3 \
			    man3/pbs_statnode.3 \
			    man3/pbs_statque.3 \
	 		    man3/pbs_statserver.3 \
			    man3/pbs_submit.3 \
			    man3/pbs_terminate.3

dist_man3_MANS = man3/rpp.3 man3/tm.3

nodist_man7_MANS = man7/pbs_job_attributes.7 \
			    man7/pbs_queue_attributes.7 \
	 		    man7/pbs_server_attributes.7 \
	 		    man7/pbs_resources_aix4.7 \
	 		    man7/pbs_resources_aix5.7 \
			    man7/pbs_resources_darwin.7 \
			    man7/pbs_resources_digitalunix.7 \
	 		    man7/pbs_resources_freebsd.7 \
			    man7/pbs_resources_fujitsu.7 \
	 		    man7/pbs_resources_hpux10.7 \
	 		    man7/pbs_resources_hpux11.7 \
			    man7/pbs_resources_irix5.7 \
	 		    man7/pbs_resources_irix6.7 \
			    man7/pbs_resources_irix6array.7 \
	 		    man7/pbs_resources_linux.7 \
			    man7/pbs_resources_netbsd.7 \
	 		    man7/pbs_resources_solaris5.7 \
			    man7/pbs_resources_solaris7.7 \
	 		    man7/pbs_resources_sp2.7 \
			    man7/pbs_resources_sunos4.7 \
	 		    man7/pbs_resources_unicos8.7 \
			    man7/pbs_resources_unicosmk2.7

nodist_man8_MANS = man8/pbs_mom.8 \
									 man8/pbsnodes.8 \
									 man8/pbs_sched_basl.8 \
									 man8/pbs_sched_cc.8 \
									 man8/pbs_sched_tcl.8 \
									 man8/pbs_server.8 \
									 man8/qdisable.8 \
									 man8/qenable.8 \
									 man8/qrun.8 \
									 man8/qstart.8 \
									 man8/qstop.8 \
									 man8/qterm.8

CLEANFILES = $(nodist_man1_MANS) $(nodist_man3_MANS) $(nodist_man7_MANS) \
	     $(nodist_man8_MANS)
MOSTLYCLEANFILES = *.gcda *.gcno *.gcov

SUFFIXES = .1.in .3.in .7.in .8.in .1 .3 .7 .8 .so

.1.in.1: soelim
	@echo "Creating $@"
	@./soelim -d $(srcdir)/ers $< > $@

.3.in.3: soelim
	@echo "Creating $@"
	@./soelim -d $(srcdir)/ers $< > $@

.7.in.7: soelim
	@echo "Creating $@"
	@./soelim -d $(srcdir)/ers $< > $@

.8.in.8: soelim
	@echo "Creating $@"
	@./soelim -d $(srcdir)/ers $< > $@

install_doc: install

install-data-hook:
	rm -f $(DESTDIR)$(man7dir)/$(program_prefix)pbs_resources$(program_suffix).7 \
	      $(DESTDIR)$(man8dir)/$(program_prefix)pbs_sched$(program_suffix).8
	@case $(PBS_MACH) in \
	  unknown) : ;; \
	  *) echo $(LN_S) $(program_prefix)pbs_resources_$(PBS_MACH)$(program_suffix).7 $(DESTDIR)$(man7dir)/$(program_prefix)pbs_resources$(program_suffix).7 ; \
	     $(LN_S) $(program_prefix)pbs_resources_$(PBS_MACH)$(program_suffix).7 $(DESTDIR)$(man7dir)/$(program_prefix)pbs_resources$(program_suffix).7 ;; \
	esac
	@case $(SCHD_TYPE) in \
	  cc|tcl|basl) echo $(LN_S) $(program_prefix)pbs_sched_$(SCHD_TYPE)$(program_suffix).8 $(DESTDIR)$(man8dir)/$(program_prefix)pbs_sched$(program_suffix).8 ; \
	               $(LN_S) $(program_prefix)pbs_sched_$(SCHD_TYPE)$(program_suffix).8 $(DESTDIR)$(man8dir)/$(program_prefix)pbs_sched$(program_suffix).8 ;; \
	  none) : ;; \
	  *) echo "Unknown scheduler type"; exit 1 ;; \
	esac

uninstall-hook:
	rm -f $(DESTDIR)$(man7dir)/$(program_prefix)pbs_resources$(program_suffix).7 $(DESTDIR)$(man8dir)/$(program_prefix)pbs_sched$(program_suffix).8


