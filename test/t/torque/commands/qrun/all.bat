#!/usr/bin/perl

use CRI::Test;
plan('no_plan');

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../../lib/";


my $testbase=$props->get_property('test.base') . "torque/commands/qrun";
setDesc("ALL qrun tests");

execute_tests("$testbase/setup.t")
  or die("Could not set up for qrun tests");

execute_tests(
              "$testbase/qrun.t",
              "$testbase/qrun_H.t",
);

execute_tests("$testbase/cleanup.t");
