#!/usr/bin/perl 

use CRI::Test;
plan('no_plan');
use strict;
use warnings;
setDesc('RELEASE pbs_sched Compatibility Tests (TORQUE 2.3)');

my $testbase = $props->get_property('test.base') . "torque/commands/pbs_sched";

execute_tests(
              "$testbase/pbs_sched.t",
             ); 
