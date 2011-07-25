#! /usr/bin/perl 

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
plan('no_plan');
setDesc('Copy Checkpoint scripts');

my $scripts_base           = "$FindBin::Bin/scripts";

my $from_checkpoint_script = "$scripts_base/blcr_checkpoint_script";
my $to_checkpoint_script   = $props->get_property('torque.checkpoint_script');

my $from_restart_script    = "$scripts_base/blcr_restart_script";
my $to_restart_script      = $props->get_property('torque.restart_script');

my $cp_cmd;

# Copy the checkpoint script
$cp_cmd = "cp -f $from_checkpoint_script $to_checkpoint_script";
runCommand($cp_cmd, test_success => 1);

# Copy the restart script
$cp_cmd = "cp -f $from_restart_script $to_restart_script";
runCommand($cp_cmd, test_success => 1);
