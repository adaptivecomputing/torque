#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;

plan('no_plan');
setDesc("ALL momctl Tests");

my $testbase = $FindBin::Bin;

execute_tests("$testbase/setup.t") 
  or die("Couldn't setup for momctl tests!");

execute_tests(
    "$testbase/momctl_c.t",
    "$testbase/momctl_C.t",
    "$testbase/momctl_d.t",
    "$testbase/momctl_q.t",
    "$testbase/momctl_r.t",
    "$testbase/momctl_s.t",
    "$testbase/momctl_f_c.t",
    "$testbase/momctl_f_C.t",
    "$testbase/momctl_f_d.t",
    "$testbase/momctl_f_q.t",
    "$testbase/momctl_f_r.t",
    "$testbase/momctl_f_s.t",
    "$testbase/momctl_h_c.t",
    "$testbase/momctl_h_C.t",
    "$testbase/momctl_h_d.t",
    "$testbase/momctl_h_q.t",
    "$testbase/momctl_h_r.t",
    "$testbase/momctl_h_s.t",
    "$testbase/momctl_p_c.t",
    "$testbase/momctl_p_C.t",
    "$testbase/momctl_p_d.t",
    "$testbase/momctl_p_q.t",
    "$testbase/momctl_p_h_d.t",
    "$testbase/momctl_p_f_d.t",
    "$testbase/momctl_p_h_c.t",
    "$testbase/momctl_p_f_c.t",
    "$testbase/momctl_p_h_C.t",
    "$testbase/momctl_p_f_C.t",
    "$testbase/momctl_p_h_q.t",
    "$testbase/momctl_p_f_q.t",
    "$testbase/momctl_p_h_r.t",
    "$testbase/momctl_p_f_r.t",
    "$testbase/momctl_p_h_s.t",
    "$testbase/momctl_p_f_s.t",
);

execute_tests("$testbase/cleanup.t"); 
