CHECK_DIRS = add_verify_resources ck_job_name cnt2server cvtdate get_server locate_job parse_at parse_depend parse_destid parse_equal parse_jobid parse_stage prepare_path prt_job_err set_attr set_resource

$(CHECK_DIRS)::
	$(MAKE) -C $@ $(MAKECMDGOALS)

check: $(CHECK_DIRS)

cleancheck:
	@for dir in $(CHECK_DIRS); do (cd $$dir && $(MAKE) clean); done
