#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../../lib/";


use CRI::Test;

use Torque::Test::Utils qw(
                            run_and_check_cmd
                          );
use Torque::Ctrl        qw(
                            stopPbsmom
                            startPbsmom
                          );


plan('no_plan');
setDesc('pbs_mom -C');

# Variables
my $checkpoint_dir = "/tmp_checkpoint";

# Commands
my $pgrep_cmd   = 'pgrep -x pbs_mom';
my $pbs_mom_cmd = "pbs_mom -C $checkpoint_dir";

# Hashes
my %pgrep;
my %pbs_mom;

# Create the checkpoint directory
run_and_check_cmd("mkdir -p $checkpoint_dir");
run_and_check_cmd("chmod 1770 $checkpoint_dir");

# Make sure pbs_mom is stopped
stopPbsmom();

# Make sure that it is stopped
%pgrep = runCommand($pgrep_cmd);
ok($pgrep{ 'EXIT_CODE' } == 1, "Verifying that pbs_mom is stopped")
  or die("Unable to continue test");

# Start pbs_mom
%pbs_mom = runCommand($pbs_mom_cmd);
ok($pbs_mom{ 'EXIT_CODE' } == 0, "Checking exit code of '$pbs_mom_cmd'")
  or diag("EXIT_CODE: $pbs_mom{ 'EXIT_CODE' }\nSTDERR: $pbs_mom{ 'STDERR' }");

# Make sure that pbs_mom has started
%pgrep = runCommand($pgrep_cmd);
ok($pgrep{ 'EXIT_CODE' } == 0, "Verifying that pbs_mom is running");

# Restart pbs_mom without the checkpoint file
diag("Restarting pbs_mom");
startPbsmom();

# delete the temporary checkpoint directory
run_and_check_cmd("rm -rf $checkpoint_dir");
