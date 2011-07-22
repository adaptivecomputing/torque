#! /usr/bin/perl 
use TestLibFinder;
use lib test_lib_loc();
 

use CRI::Test;
plan('no_plan'); 
setDesc('make install');

runCommand("cd $build_dir; $cmd", test_success_die => 1);
