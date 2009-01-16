#!/usr/bin/perl

use CRI::Test;
plan('no_plan');

use strict;
use warnings;

my $testbase=$props->get_property('test.base') . "torque/commands/qsig";
setDesc("ALL qsig tests");

execute_tests("$testbase/setup.t")
 or die("Could not set up for qsig tests");

execute_tests(
              "$testbase/qsig.t",
              "$testbase/qsig_s.t",
);

execute_tests("$testbase/cleanup.t");
