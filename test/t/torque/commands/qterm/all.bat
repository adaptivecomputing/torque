#!/usr/bin/perl 

use CRI::Test;
plan('no_plan');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

setDesc('ALL qterm Compatibility Tests');

my $testbase = $props->get_property('test.base') . "torque/commands/qterm";

execute_tests("$testbase/setup.t") 
  or die('Could not setup qterm tests');

execute_tests(
              "$testbase/qterm.t",
              "$testbase/qterm_t_quick.t",
              "$testbase/checkpoint/qterm_t_immediate.t",
              "$testbase/checkpoint/qterm_t_delay.t",
              "$testbase/rerunable/qterm_t_immediate.t",
              "$testbase/rerunable/qterm_t_delay.t",
             ); 

execute_tests("$testbase/cleanup.t"); 
