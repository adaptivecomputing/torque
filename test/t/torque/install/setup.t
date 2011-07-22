#!/usr/bin/perl
#* This test runs torque.setup which runs pbs_server -t create
#* and runs a series of qmgr commands to define admins, create queues, etc.
#* Finally, it shuts torque back down.
use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();
 
use CRI::Test;
use Torque::Ctrl qw(
                    stopTorque
                   );
plan('no_plan');
setDesc('Setup Torque with multiple queues');

# Scripts and paramters
my $build_dir   = test_lib_loc().'/../..';
my $torque_queue_setup_script = "$FindBin::Bin/scripts/torque.queue.setup";
my $queue1                    = $props->get_property('torque.queue.one');
my $queue2                    = $props->get_property('torque.queue.two');

my $setup_cmd = "$torque_queue_setup_script root $queue1 $queue2";
runCommand("cd $build_dir; $setup_cmd", test_success => 1);

# Shutdown torque
stopTorque();
