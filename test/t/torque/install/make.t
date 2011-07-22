#!/usr/bin/perl

use TestLibFinder;
use lib test_lib_loc();
 
use CRI::Test;
plan('no_plan'); 
setDesc('make');

use strict;
use warnings;

my %cmd_result = runCommand("cd $build_dir; $cmd", test_success_die => 1);
