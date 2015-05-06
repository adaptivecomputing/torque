include $(top_srcdir)/buildutils/config.mk

NUMA_DIRS = allocation machine numa_chip numa_core numa_socket numa_pci_device numa_socket

CHECK_DIRS = scaffold_fail torque_test_lib ${NUMA_DIRS}

$(CHECK_DIRS)::
	$(MAKE) -C $@ $(MAKECMDGOALS)

check: $(CHECK_DIRS)
