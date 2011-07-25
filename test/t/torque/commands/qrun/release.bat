#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

plan('no_plan');
setDesc("RELEASE qrun Compatibility Test");

my $testbase = $FindBin::Bin;


execute_tests("$testbase/setup.t")
  or die("Could not set up for qrun tests");

execute_tests(
              "$testbase/qrun.t",
              "$testbase/qrun_cH.t",
);

#execute_tests("$testbase/cleanup.t");
