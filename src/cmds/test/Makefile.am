#Files that are used for wrappers in order to test the code.
#qsub wraps qsub_functions
CHECK_DIRS = MXML common_cmds pbs_track pbsdsh pbsnodes pbspd pbspoe qalter qchkpt qdel qdisable qenable qgpumode qgpureset qhold qmgr qmove qmsg qorder qrerun qrls qrun qselect qsig qstart qstat qstop qsub_functions qterm

$(CHECK_DIRS)::
	$(MAKE) -C $@ $(MAKECMDGOALS)

check: $(CHECK_DIRS)

cleancheck:
	@for dir in $(CHECK_DIRS); do (cd $$dir && $(MAKE) clean); done
