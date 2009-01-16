#!/usr/bin/perl

use CRI::Test;

plan('no_plan');
use strict;
use warnings;

my $testbase=$props->get_property('test.base') . "torque/commands/qdel";
setDesc("RELEASE qdel Compatibility Tests");

execute_tests("$testbase/setup.t") 
  or die("Couldn't setup for qdel tests!");

execute_tests(
              "$testbase/qdel.t",
              "$testbase/qdel_p.t",
              "$testbase/qdel_m.t",
              "$testbase/qdel_W.t",
);

execute_tests("$testbase/cleanup.t"); 
