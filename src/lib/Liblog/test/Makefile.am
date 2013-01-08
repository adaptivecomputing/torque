CHECK_DIRS = chk_file_sec log_event pbs_log pbs_messages setup_env

$(CHECK_DIRS)::
	$(MAKE) -C $@ $(MAKECMDGOALS)

check: $(CHECK_DIRS)
	
cleancheck:
	@for dir in $(CHECK_DIRS); do (cd $$dir && $(MAKE) clean); done
