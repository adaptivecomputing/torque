#!/usr/bin/perl 

use CRI::Test;
plan('no_plan');
use strict;
use warnings;
setDesc('RELEASE qterm Compatibility Tests (TORQUE 2.3)');

my $testbase = $props->get_property('test.base') . "torque/commands/qterm";

execute_tests("$testbase/setup.t") 
  or die('Could not cleanup tests');

execute_tests(
              "$testbase/qterm.t",
              "$testbase/qterm_t_quick.t",
              "$testbase/rerunable/qterm_t_immediate.t",
              "$testbase/rerunable/qterm_t_delay.t"
             ); 

execute_tests("$testbase/cleanup.t"); 
