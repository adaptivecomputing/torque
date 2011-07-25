#!/usr/bin/perl 

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

plan('no_plan');
setDesc('RELEASE Torque Compatability Tests');

my $testbase = $FindBin::Bin;

execute_tests(
              "$testbase/momctl/release.bat",
              "$testbase/qalter/release.bat",
              "$testbase/qchkpt/release.bat",
              "$testbase/qdel/release.bat",
              "$testbase/qhold/release.bat",
              "$testbase/qmgr/release.bat",
              "$testbase/qrerun/release.bat",
              "$testbase/qrls/release.bat",
              "$testbase/qrun/release.bat",
              "$testbase/qsig/release.bat",
              "$testbase/qstat/release.bat",
              "$testbase/qsub/release.bat",
              "$testbase/qterm/release.bat",
              "$testbase/tracejob/release.bat",
             ); 

# Tests that can make the environment unstable if they fail.  We want to limit
# their impact on the test suite
execute_tests(
              "$testbase/pbsnodes/release.bat",
              "$testbase/pbs_sched/release.bat",
              "$testbase/pbs_mom/release.bat",
              "$testbase/pbs_server/release.bat",
             );
