#!/usr/bin/perl 

use CRI::Test;
plan('no_plan');
setDesc('RELEASE Torque 2.3 Compatibility Tests');
use strict;
use warnings;

my $testbase = $props->get_property('test.base') . "torque/commands";
execute_tests(
              "$testbase/momctl/release.bat",
              "$testbase/qalter/release2.3.bat",
#              "$testbase/qchkpt/release.bat",
              "$testbase/qdel/release.bat",
              "$testbase/qhold/release2.3.bat",
              "$testbase/qmgr/release.bat",
              "$testbase/qrerun/release.bat",
              "$testbase/qrls/release.bat",
              "$testbase/qrun/release.bat",
              "$testbase/qsig/release.bat",
              "$testbase/qstat/release.bat",
              "$testbase/qsub/release2.3.bat",
              "$testbase/qterm/release2.3.bat",
              "$testbase/tracejob/release.bat",
             );

# Tests that can make the environment unstable if they fail.  We want to limit
# their impact on the test suite
execute_tests(
              "$testbase/pbs_mom/release2.3.bat",
              "$testbase/pbsnodes/release.bat",
              "$testbase/pbs_sched/release2.3.bat",
              "$testbase/pbs_server/release2.3.bat",
             );
