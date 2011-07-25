#! /usr/bin/perl 

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

plan('no_plan'); 
setDesc('Kill Moab');

my $pgrep_cmd = 'pgrep -x moab';
my $pkill_cmd = 'pkill -x moab';

# Kill moab if running
if (! runCommand($pgrep_cmd))
  {

  my %result = runCommand($pkill_cmd);

  cmp_ok($result{ 'EXIT_CODE' }, 'eq', 0, "Checking exit code of '$pkill_cmd'");

  } # END if (! runCommand($pgrep_cmd))
else
  {

  pass("Moab doesn't appear to be running");

  } # END else
