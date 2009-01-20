#! /usr/bin/perl 
#* This test runs torque.setup which runs pbs_server -t create
#* and runs a series of qmgr commands to define admins, create queues, etc.
#* Finally, it shuts torque back down.

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";

use CRI::Test;
use CRI::Utils   qw(
                     run_and_check_cmd
                   );
use Torque::Ctrl qw(
                    stopTorque
                   );

plan('no_plan');
setDesc('Setup Torque with multiple queues');

# Scripts and paramters
my $torque_queue_setup_script = "$FindBin::Bin/scripts/torque.queue.setup";
my $queue1                    = $props->get_property('torque.queue.one');
my $queue2                    = $props->get_property('torque.queue.two');

# Extract build directory from test properties
ok(-e $torque_queue_setup_script, "Checking if '$torque_queue_setup_script' exists") 
  or die("'$torque_queue_setup_script' doesn't exist");

# Run setup
my $setup_cmd = "$torque_queue_setup_script root $queue1 $queue2";
run_and_check_cmd($setup_cmd);

# Shutdown torque
stopTorque
