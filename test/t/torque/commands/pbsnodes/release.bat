#!/usr/bin/perl
use CRI::Test;
plan('no_plan');
use strict;
use warnings;

my $testbase=$props->get_property('test.base') . "torque/commands/pbsnodes";
setDesc("RELEASE pbsnodes Compatibility Tests");

execute_tests("$testbase/setup.t") 
  or die("Couldn't setup for pbsnodes tests!");

execute_tests(
    "$testbase/pbsnodes.t",
    "$testbase/pbsnodes_a.t",
    "$testbase/pbsnodes_c.t",
#    "$testbase/pbsnodes_d.t", # Not implemented yet
    "$testbase/pbsnodes_l.t",
    "$testbase/pbsnodes_o.t",
#    "$testbase/pbsnodes_p.t", # Not implemented yet
    "$testbase/pbsnodes_q.t",
    "$testbase/pbsnodes_s.t",
    "$testbase/pbsnodes_x.t",
    "$testbase/pbsnodes_a_q.t",
    "$testbase/pbsnodes_c_q.t",
    "$testbase/pbsnodes_l_q.t",
    "$testbase/pbsnodes_s_q.t",
    "$testbase/pbsnodes_x_q.t",
    "$testbase/pbsnodes_l_n.t",
    "$testbase/pbsnodes_o_N.t",
    "$testbase/pbsnodes_s_a.t",
    "$testbase/pbsnodes_s_c.t",
    "$testbase/pbsnodes_s_l.t",
    "$testbase/pbsnodes_s_x.t",
    "$testbase/pbsnodes_s_a_q.t",
    "$testbase/pbsnodes_s_c_q.t",
    "$testbase/pbsnodes_s_l_q.t",
    "$testbase/pbsnodes_s_x_q.t",
);

execute_tests("$testbase/cleanup.t"); 
