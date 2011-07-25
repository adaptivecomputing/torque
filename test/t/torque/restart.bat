#!/usr/bin/perl 
use CRI::Test;
plan('no_plan');
setDesc("Restart Torque");
use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


my $testbase = $FindBin::Bin;


execute_tests("$testbase/uninstall/shutdown.t","$testbase/install/startup.t");
