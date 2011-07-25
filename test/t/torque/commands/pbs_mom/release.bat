#!/usr/bin/perl 

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

plan('no_plan');
setDesc('RELEASE pbs_mom Compatibility Tests');

my $testbase = $FindBin::Bin;

execute_tests("$testbase/setup.t") 
  or die('Could not setup pbs_mom tests');

execute_tests(
              "$testbase/pbs_mom.t",
              "$testbase/pbs_mom_a.t",
              "$testbase/pbs_mom_c.t",
              "$testbase/pbs_mom_cC.t",
              "$testbase/pbs_mom_d.t",
#              "$testbase/pbs_mom_h.t",         # This is not implemented
              "$testbase/pbs_mom_cL.t",
              "$testbase/pbs_mom_cM.t",
              "$testbase/pbs_mom_cR.t",
              "$testbase/pbs_mom_default_p.t",
              "$testbase/pbs_mom_p.t",
              "$testbase/pbs_mom_q.t",
# RT5530              "$testbase/pbs_mom_r.t",
              "$testbase/pbs_mom_x.t",
             ); 

execute_tests("$testbase/cleanup.t"); 

