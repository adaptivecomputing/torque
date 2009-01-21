#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;
plan('no_plan');
setDesc("ALL qalter Tests");

my $testbase = $FindBin::Bin;

execute_tests("$testbase/setup.t") 
  or die("Couldn't setup for qalter tests!");

execute_tests(
              "$testbase/qalter_a.t",
              "$testbase/qalter_A.t",
              "$testbase/qalter_c.t",
              "$testbase/qalter_e.t",
              "$testbase/qalter_h.t",
              "$testbase/qalter_j.t",
              "$testbase/qalter_k.t",
              "$testbase/qalter_l.t",
              "$testbase/qalter_m.t",
              "$testbase/qalter_M.t",
              "$testbase/qalter_N.t",
              "$testbase/qalter_o.t",
              "$testbase/qalter_p.t",
              "$testbase/qalter_r.t",
              "$testbase/qalter_S.t",
              "$testbase/qalter_u.t",
              "$testbase/qalter_W_sync.t",
              "$testbase/qalter_W_after.t",
              "$testbase/qalter_W_before.t",
              "$testbase/qalter_W_group_list.t",
              "$testbase/qalter_W_data_staging.t",
);

execute_tests("$testbase/cleanup.t"); 
