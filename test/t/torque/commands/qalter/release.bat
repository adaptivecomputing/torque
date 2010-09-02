#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;
plan('no_plan');
setDesc("RELEASE qalter Compatibility Tests");

my $testbase = $FindBin::Bin;

execute_tests("$testbase/setup.t") 
  or die("Couldn't setup for qalter tests!");

execute_tests(
# RT8017              "$testbase/qalter_a.t",
              "$testbase/qalter_cA.t",
              "$testbase/qalter_c.t",
              "$testbase/qalter_e.t",
              "$testbase/qalter_h.t",
              "$testbase/qalter_j.t",
              "$testbase/qalter_k.t",
              "$testbase/qalter_l.t",
              "$testbase/qalter_m.t",
              "$testbase/qalter_cM.t",
              "$testbase/qalter_cN.t",
              "$testbase/qalter_o.t",
              "$testbase/qalter_p.t",
              "$testbase/qalter_r.t",
              "$testbase/qalter_cS.t",
# JR-TRQ-2              "$testbase/qalter_t_p.t",
              "$testbase/qalter_u.t",
              "$testbase/qalter_cW_sync.t",
              "$testbase/qalter_cW_after.t",
#              "$testbase/qalter_cW_before.t",
              "$testbase/qalter_cW_group_list.t",
              "$testbase/qalter_cW_data_staging.t",
);

execute_tests("$testbase/cleanup.t"); 
