#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;

plan('no_plan');
setDesc("ALL qmgr tests");

my $testbase = $FindBin::Bin;

execute_tests("$testbase/setup.t") 
  or die("Couldn't setup for qmgr tests!");

execute_tests(
              "$testbase/qmgr_c_scheduling.t",
              "$testbase/qmgr_c_queue.t",
);

execute_tests("$testbase/cleanup.t"); 
