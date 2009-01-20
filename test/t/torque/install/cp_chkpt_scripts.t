#! /usr/bin/perl 

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";

use CRI::Test;
use CRI::Utils qw(
                   run_and_check_cmd
                 );

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
run_and_check_cmd($cp_cmd);

# Copy the restart script
$cp_cmd = "cp -f $from_restart_script $to_restart_script";
run_and_check_cmd($cp_cmd);
