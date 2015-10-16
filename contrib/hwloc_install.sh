#!/bin/bash

# script to help install hwloc for those that would like help
# run this script from a folder where you don't mind installing hwloc

# must be version $ver or higher, so download and install that 
ver=1.9

#download
wget http://www.open-mpi.org/software/hwloc/v1.9/downloads/hwloc-${ver}.tar.gz

#un-tar the file
tar -xzf hwloc-${ver}.tar.gz

#configure and install 
cd hwloc-${ver}
./configure
make 
make install 

