include $(top_srcdir)/buildutils/config.mk

CHECK_DIRS = scaffold_fail torque_test_lib

$(CHECK_DIRS)::
	$(MAKE) -C $@ $(MAKECMDGOALS)

check: $(CHECK_DIRS)
