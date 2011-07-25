#!/usr/bin/perl 

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

plan('no_plan');
setDesc('RELEASE pbs_server Compatibility Tests');

my $testbase = $FindBin::Bin;

execute_tests("$testbase/setup.t") 
  or die('Could not setup pbs_server tests');

execute_tests(
              "$testbase/pbs_server.t",
              "$testbase/pbs_server_a.t",
              "$testbase/pbs_server_cA.t",
              "$testbase/pbs_server_d.t",
#              "$testbase/pbs_server_h.t", # Currently just spits out a usage message
              "$testbase/pbs_server_cL.t",
              "$testbase/pbs_server_cM.t",
              "$testbase/pbs_server_p.t",
              "$testbase/pbs_server_cR.t",
              "$testbase/pbs_server_cS.t",
#              "$testbase/pbs_server_t_create.t",
#              "$testbase/rerunable/pbs_server_t_hot.t",
#              "$testbase/rerunable/pbs_server_t_warm.t",
              "$testbase/rerunable/pbs_server_t_cold.t",
#              "$testbase/checkpointable/pbs_server_t_hot.t",
#              "$testbase/checkpointable/pbs_server_t_warm.t",
              "$testbase/checkpointable/pbs_server_t_cold.t",
             ); 

execute_tests("$testbase/cleanup.t"); 

