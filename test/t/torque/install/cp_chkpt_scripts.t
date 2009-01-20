#! /usr/bin/perl 

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";

use CRI::Test;

plan('no_plan');
setDesc('Configure Torque on the Compute Nodes');

my $scripts_base           = $props->get_property('test.base') . "torque/install/scripts";

my $from_checkpoint_script = "$scripts_base/blcr_checkpoint_script";
my $to_checkpoint_script   = $props->get_property('torque.checkpoint_script');

my $from_restart_script    = "$scripts_base/blcr_restart_script";
my $to_restart_script      = $props->get_property('torque.restart_script');

my $cp_cmd;
my %cp;

# Copy the checkpoint script
$cp_cmd = "cp -f $from_checkpoint_script $to_checkpoint_script";
%cp     = runCommand($cp_cmd);
ok($cp{ 'EXIT_CODE' } == 0, "Checking exit status of '$cp_cmd'");

# Copy the restart script
$cp_cmd = "cp -f $from_restart_script $to_restart_script";
%cp     = runCommand($cp_cmd);
ok($cp{ 'EXIT_CODE' } == 0, "Checking exit status of '$cp_cmd'");

