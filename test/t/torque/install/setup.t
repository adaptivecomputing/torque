#!/usr/bin/perl

use TestLibFinder;
use lib test_lib_loc();
 
#* This test runs torque.setup which runs pbs_server -t create
#* and runs a series of qmgr commands to define admins, create queues, etc.
#* Finally, it shuts torque back down.
use CRI::Test;
plan('no_plan');
setDesc('Setup Torque');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";

use CRI::Test;
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
ok(-d $props->get_property('torque.build.dir'),"Checking if torque build directory exists") or die("Torque build directory doesn't exist");

# Change directory to build dir
ok(chdir $props->get_property('torque.build.dir'),"Changing directory to " . $props->get_property('torque.build.dir')) or die("Can't change to torque build dir");

# Run setup
my $setup_cmd = "$torque_queue_setup_script root $queue1 $queue2";
runCommand($setup_cmd, test_success => 1);

# Shutdown torque
stopTorque();
