#!/usr/bin/perl

use CRI::Test;
plan('no_plan');
use strict;
use warnings;

my $testbase=$props->get_property('test.base') . "torque/commands/qhold";
setDesc("ALL qhold Tests");

execute_tests("$testbase/setup.t") 
  or die("Couldn't setup for qhold tests!");

execute_tests(
              "$testbase/checkpoint/all.bat",
              "$testbase/non-checkpoint/all.bat",
);

execute_tests("$testbase/cleanup.t"); 
