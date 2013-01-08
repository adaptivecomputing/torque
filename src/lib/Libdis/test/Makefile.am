CHECK_DIRS = discui_ discul_ disi10d_ disi10l_ disiui_ disp10d_ disp10l_ disrcs disrd disrf disrfcs disrfst disrl disrl_ disrsc disrsi disrsi_ disrsl disrsl_ disrss disrst disruc disrui disrul disrus diswcs diswf diswl_ diswsi diswsl diswui diswui_ diswul

$(CHECK_DIRS)::
	$(MAKE) -C $@ $(MAKECMDGOALS)

check: $(CHECK_DIRS)
	
cleancheck:
	@for dir in $(CHECK_DIRS); do (cd $$dir && $(MAKE) clean); done
