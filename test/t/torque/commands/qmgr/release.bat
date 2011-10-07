#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

plan('no_plan');
setDesc("RELEASE qmgr Compatibility Tests");

my $testbase = $FindBin::Bin;

execute_tests("$testbase/setup.t") 
  or die("Couldn't setup for qmgr tests!");

execute_tests(
              "$testbase/qmgr_c_scheduling.t",
              "$testbase/qmgr_c_queue.t",
              "$testbase/qmgr_c_server_max_slot_limit.t",
);

execute_tests("$testbase/cleanup.t"); 
