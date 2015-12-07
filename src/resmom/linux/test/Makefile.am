SUBDIRS = numa_node node_internals 
if BUILD_L26_CPUSETS
SUBDIRS += cpuset
endif

cleancheck:
	@for dir in $(SUBDIRS); do (cd $$dir && $(MAKE) clean); done
