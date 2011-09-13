include $(top_srcdir)/buildutils/config.mk

if USING_TCL
TEST_TCL = pbsTclInit
endif

if USING_TK
TEST_TK = pbsTkInit
endif

SUBDIRS = chk_tree hostn $(TEST_TCL) $(TEST_TK) printjob printserverdb printtracking tracejob
