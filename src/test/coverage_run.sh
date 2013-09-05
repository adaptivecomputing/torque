#!/bin/bash

# this file performs gcov output generation in the current
# directory.

# ASSUMPTION:  the name of the current directory is the name
#              of the file whose gcov data is to be generated

F=`pwd`

FILENAME=`basename $F`

if [ "x$FILENAME" = "xMAssertAbort" ] ; then
  FILENAME=MAssert
fi

if [ "x$FILENAME" = "xMAssertNonAbort" ] ; then
  FILENAME=MAssert
fi

echo "File: $FILENAME"

cp -p .libs/${FILENAME}.gc* . 

# Extract the gcov results for the UUT
RESULTS=$(gcov ${FILENAME} | grep -A 2 ${FILENAME}.c)

# Generate the file name of the results
FILE=$(echo $RESULTS | awk ' { print $2 }' | sed -e "s/'//g")
FILENAME=$(basename $FILE)

# Generate the coverage percentage of the results
PERCENT=$(echo $RESULTS | awk ' { print $4 }')

# Generate the Line count of the results
LINES=$(echo $RESULTS | awk ' { print $6 }')

# Generate the final output string
PARSED_RESULT="TOTALCOV -- ${FILENAME}: Lines(${LINES}) - ${PERCENT}"

# Output the results
echo -e "\033[1;33m$PARSED_RESULT\033[0m"
                      
exit 0
  
