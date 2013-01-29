include ../Makefile.ut

libuut_la_SOURCES =  ${PROG_ROOT}/array_func.c

coverage_run.sh:
	echo 'cp -p .libs/array_func.gc* . >/dev/null 2>&1' > $@
	echo 'RESULTS=($$(gcov array_func.gcda))' >> $@
	echo 'PARSED_RESULT="TOTALCOV -- $${RESULTS[1]}: Lines($${RESULTS[5]})- $${RESULTS[3]}"' >> $@
	echo 'echo -e "\033[40m\033[1;33m$$PARSED_RESULT\033[0m"' >> $@
	chmod +x $@
