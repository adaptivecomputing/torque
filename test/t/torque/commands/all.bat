#!/usr/bin/perl 

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";

use CRI::Test;

plan('no_plan');
setDesc('ALL Torque Compatibility Tests');

my $testbase = $FindBin::Bin;

execute_tests(
              "$testbase/momctl/all.bat",
              "$testbase/qalter/all.bat",
              "$testbase/qchkpt/all.bat",
              "$testbase/qdel/all.bat",
              "$testbase/qhold/all.bat",
              "$testbase/qmgr/all.bat",
              "$testbase/qrerun/all.bat",
              "$testbase/qrls/all.bat",
              "$testbase/qrun/all.bat",
              "$testbase/qsig/all.bat",
              "$testbase/qstat/all.bat",
              "$testbase/qsub/all.bat",
              "$testbase/qterm/all.bat",
              "$testbase/tracejob/all.bat",
             ); 

# Tests that can make the environment unstable if they fail.  We want to limit
# their impact on the test suite
execute_tests(
              "$testbase/pbsnodes/all.bat",
              "$testbase/pbs_sched/all.bat",
              "$testbase/pbs_mom/all.bat",
              "$testbase/pbs_server/all.bat",
             );
