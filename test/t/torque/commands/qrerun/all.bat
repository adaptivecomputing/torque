#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

plan('no_plan');
setDesc("ALL qrerun tests");

my $testbase = $FindBin::Bin;

execute_tests("$testbase/setup.t") 
  or die("Couldn't setup for qrerun tests!");

execute_tests(
              "$testbase/qrerun.t",
              "$testbase/qrerun_f.t",
);

execute_tests("$testbase/cleanup.t"); 
