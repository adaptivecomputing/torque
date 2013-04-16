#!/bin/bash

# Argument 1 is the input file name
INPUTF=$1

# The basil version really shouldn't matter here
BASILVER=1.1

if [[ -n "$INPUTF" && -r $INPUTF ]]; then
	MODE=$(head -n1 $INPUTF)
	tail -n +2 $INPUTF > ${INPUTF}.new
	mv ${INPUTF}.new $INPUTF
fi

if [[ -z "$MODE" && "$1" == "f" ]]; then
	MODE=f
fi

if [ -z "$MODE" ]; then
	MODE=s
fi

case $MODE in
	f)
		echo "<?xml version='1.0'?>"
		echo "<BasilResponse protocol='$BASILVER'>"
		echo "<ResponseData status='FAILURE' method='RELEASE' error_class='PERMANENT' error_source='BACKEND'>"
		echo "<Message severity='ERROR'>ALPS error: apsched: No entry for resId 9</Message>"
		echo "</ResponseData>"
		echo "</BasilResponse>"
		exit 0
		;;
	*)
		echo "<?xml version='1.0'?><BasilResponse protocol='$BASILVER'> <ResponseData status='SUCCESS' method='RELEASE'> <Released/></ResponseData></BasilResponse>"
		exit 0
		;;
esac
