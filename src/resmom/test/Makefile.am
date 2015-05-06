CHECK_DIRS = alps_reservations catch_child checkpoint cray_energy generate_alps_status \
						 mom_comm mom_inter mom_job_func mom_main mom_process_request \
						 mom_req_quejob mom_server parse_config pbs_demux prolog release_reservation \
						 requests start_exec tmsock_recov trq_cgroups

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
