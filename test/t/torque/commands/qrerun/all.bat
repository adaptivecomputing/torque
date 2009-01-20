#!/usr/bin/perl

use CRI::Test;
plan('no_plan');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../../lib/";


my $testbase=$props->get_property('test.base') . "torque/commands/qrerun";
setDesc("ALL qrerun tests");

execute_tests("$testbase/setup.t") 
  or die("Couldn't setup for qrerun tests!");

execute_tests(
              "$testbase/qrerun.t",
              "$testbase/qrerun_f.t",
);

execute_tests("$testbase/cleanup.t"); 
