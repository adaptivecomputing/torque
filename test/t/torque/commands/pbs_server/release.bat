#!/usr/bin/perl 

use CRI::Test;
plan('no_plan');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

setDesc('RELEASE pbs_server Compatibility Tests');

my $testbase = $props->get_property('test.base') . "torque/commands/pbs_server";

execute_tests("$testbase/setup.t") 
  or die('Could not setup pbs_server tests');

execute_tests(
              "$testbase/pbs_server.t",
              "$testbase/pbs_server_a.t",
              "$testbase/pbs_server_A.t",
              "$testbase/pbs_server_d.t",
#              "$testbase/pbs_server_h.t", # Currently just spits out a usage message
              "$testbase/pbs_server_L.t",
              "$testbase/pbs_server_M.t",
              "$testbase/pbs_server_p.t",
              "$testbase/pbs_server_R.t",
              "$testbase/pbs_server_S.t",
#              "$testbase/pbs_server_t_create.t",
#              "$testbase/rerunable/pbs_server_t_hot.t",
#              "$testbase/rerunable/pbs_server_t_warm.t",
              "$testbase/rerunable/pbs_server_t_cold.t",
#              "$testbase/checkpointable/pbs_server_t_hot.t",
#              "$testbase/checkpointable/pbs_server_t_warm.t",
              "$testbase/checkpointable/pbs_server_t_cold.t",
             ); 

execute_tests("$testbase/cleanup.t"); 

