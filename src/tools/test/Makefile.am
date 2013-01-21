include $(top_srcdir)/buildutils/config.mk

if USING_TCL
TEST_TCL = pbsTclInit
endif

if USING_TK
TEST_TK = pbsTkInit
endif

CHECK_DIRS = chk_tree hostn $(TEST_TCL) $(TEST_TK) printjob printserverdb printtracking tracejob

$(CHECK_DIRS)::
	$(MAKE) -C $@ $(MAKECMDGOALS)

check: $(CHECK_DIRS)

cleancheck:
	@for dir in $(CHECK_DIRS); do (cd $$dir && $(MAKE) clean); done
