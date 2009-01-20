#!/usr/bin/perl 

use CRI::Test;
plan('no_plan');
setDesc('FULL Torque Compatibility Tests');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";


my $testbase = $FindBin::Bin;

execute_tests(
#              "$testbase/momctl/snap.bat",
#              "$testbase/qalter/snap.bat",
#              "$testbase/qchkpt/snap.bat",
#              "$testbase/qdel/snap.bat",
#              "$testbase/qhold/snap.bat",
#              "$testbase/qmgr/snap.bat",
#              "$testbase/qrerun/snap.bat",
#              "$testbase/qrls/snap.bat",
#              "$testbase/qrun/snap.bat",
#              "$testbase/qsig/snap.bat",
#              "$testbase/qstat/snap.bat",
#              "$testbase/qsub/snap.bat",
#              "$testbase/qterm/snap.bat",
#              "$testbase/tracejob/snap.bat",
             ); 

# Tests that can make the environment unstable if they fail.  We want to limit
# their impact on the test suite
execute_tests(
#              "$testbase/pbsnodes/snap.bat",
#              "$testbase/pbs_sched/snap.bat",
#              "$testbase/pbs_mom/snap.bat",
#              "$testbase/pbs_server/snap.bat",
             );
