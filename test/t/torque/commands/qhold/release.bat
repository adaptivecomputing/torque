#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
plan('no_plan');
setDesc("RELEASE qhold Compatibility Tests");

my $testbase = $FindBin::Bin;

execute_tests("$testbase/setup.t") 
  or die("Couldn't setup for qhold tests!");

execute_tests(
# needs BLCR              "$testbase/checkpoint/release.bat",
              "$testbase/non-checkpoint/release.bat",
);

execute_tests("$testbase/cleanup.t"); 
