#!/usr/bin/perl
use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();
 
use CRI::Test;
plan('no_plan'); 
setDesc('make');

my $build_dir   = test_lib_loc().'/../..';

my %cmd_result = runCommand("cd $build_dir; make -j 32", test_success_die => 1);
