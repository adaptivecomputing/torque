#!/usr/bin/perl
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;

plan('no_plan');
setDesc("ALL pbsnodes Tests");

my $testbase = $FindBin::Bin;

execute_tests("$testbase/setup.t") 
  or die("Couldn't setup for pbsnodes tests!");

execute_tests(
    "$testbase/pbsnodes.t",
    "$testbase/pbsnodes_a.t",
    "$testbase/pbsnodes_c.t",
    "$testbase/pbsnodes_d.t",
    "$testbase/pbsnodes_l.t",
    "$testbase/pbsnodes_o.t",
    "$testbase/pbsnodes_p.t",
    "$testbase/pbsnodes_q.t",
    "$testbase/pbsnodes_s.t",
    "$testbase/pbsnodes_x.t",
    "$testbase/pbsnodes_a_q.t",
    "$testbase/pbsnodes_c_q.t",
    "$testbase/pbsnodes_l_q.t",
    "$testbase/pbsnodes_s_q.t",
    "$testbase/pbsnodes_x_q.t",
    "$testbase/pbsnodes_l_n.t",
    "$testbase/pbsnodes_o_cN.t",
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
