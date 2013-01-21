CHECK_DIRS = conn_table get_hostaddr get_hostname md5 net_client net_common net_server net_set_clse port_forwarding rm server_core

$(CHECK_DIRS)::
	$(MAKE) -C $@ $(MAKECMDGOALS)
	
check: $(CHECK_DIRS)
	
cleancheck:
	@for dir in $(CHECK_DIRS); do (cd $$dir && $(MAKE) clean); done
