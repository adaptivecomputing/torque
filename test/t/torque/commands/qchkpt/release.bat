#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

plan('no_plan');
setDesc("RELEASE qchckpt Compatibility Tests");

my $testbase = $FindBin::Bin;

execute_tests("$testbase/setup.t") 
  or die("Couldn't setup for qchkpt tests!");

execute_tests(
              "$testbase/qchkpt.t",
#              "$testbase/mult_node_qchkpt.t",
);

execute_tests("$testbase/cleanup.t"); 
