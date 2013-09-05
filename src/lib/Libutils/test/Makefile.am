<<<<<<< HEAD
CHECK_DIRS = u_MXML u_dynamic_string u_groups u_hash_map_structs u_hash_table u_lock_ctl u_memmgr u_mom_hierarchy u_mu u_putenv u_resizable_array u_threadpool u_tree u_users u_xml
=======
CHECK_DIRS = u_MXML u_groups u_hash_map_structs u_hash_table u_lock_ctl u_memmgr u_mom_hierarchy u_mu u_putenv u_resizable_array u_threadpool u_tree u_users u_xml
>>>>>>> master

$(CHECK_DIRS)::
	$(MAKE) -C $@ $(MAKECMDGOALS)

check: $(CHECK_DIRS)

cleancheck:
	@for dir in $(CHECK_DIRS); do (cd $$dir && $(MAKE) clean); done
