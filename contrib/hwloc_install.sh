#!/bin/bash

# script to help install hwloc for those that would like help
# run this script from a folder where you don't mind installing hwloc

# must be version 1.1 or higher, so download and install that 

#download
wget http://www.open-mpi.org/software/hwloc/v1.1/downloads/hwloc-1.1.tar.gz

#un-tar the file
tar -xzf hwloc-1.1.tar.gz

#configure and install 
cd hwloc-1.1
./configure
make 
make install 

