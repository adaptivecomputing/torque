#! /usr/bin/perl 
#* This test runs torque.setup which runs pbs_server -t create
#* and runs a series of qmgr commands to define admins, create queues, etc.
#* Finally, it shuts torque back down.

use strict;
use warnings;

use CRI::Test;

plan('no_plan');
setDesc('Setup Torque with multiple queues');

# Scripts and paramters
my $torque_queue_setup_script = $props->get_property('test.base') . "/torque/install/scripts/torque.queue.setup";
my $queue1                    = $props->get_property('torque.queue.one');
my $queue2                    = $props->get_property('torque.queue.two');

# Extract build directory from test properties
ok(-e $torque_queue_setup_script, "Checking if '$torque_queue_setup_script' exists") 
  or die("'$torque_queue_setup_script' doesn't exist");

# Run setup
my $setup_cmd = "$torque_queue_setup_script root $queue1 $queue2";
my %setup     = runCommand($setup_cmd);
ok($setup{ 'EXIT_CODE' } == 0, "Checking exit code of '$setup_cmd'")
  or die("Torque queue setup script failed");

# Shutdown torque
runCommand("qterm -t quick") 
  && die("Couldn't shut down torque!");
