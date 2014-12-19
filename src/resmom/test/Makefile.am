CHECK_DIRS = catch_child checkpoint mom_job_func mom_comm mom_inter mom_main mom_server pbs_demux mom_process_request prolog mom_req_quejob requests start_exec tmsock_recov alps_reservations generate_alps_status release_reservation cray_energy

if MIC
CHECK_DIRS += mic
endif

if BUILDCPA
CHECK_DIRS += cray_cpa
endif

$(CHECK_DIRS)::
	$(MAKE) -C $@ $(MAKECMDGOALS)

check: $(CHECK_DIRS)

cleancheck:
	@for dir in $(CHECK_DIRS); do (cd $$dir && $(MAKE) clean); done
