CHECK_DIRS = u_MXML u_groups u_hash_map_structs u_lock_ctl u_misc u_mom_hierarchy u_mu u_mutex_mgr u_putenv u_threadpool u_tree u_users u_xml

$(CHECK_DIRS)::
	$(MAKE) -C $@ $(MAKECMDGOALS)

check: $(CHECK_DIRS)

cleancheck:
	@for dir in $(CHECK_DIRS); do (cd $$dir && $(MAKE) clean); done
