#! /usr/bin/perl 
#* This test runs torque.setup which runs pbs_server -t create
#* and runs a series of qmgr commands to define admins, create queues, etc.
#* Finally, it shuts torque back down.

use CRI::Test;
plan('no_plan');
setDesc('Setup Torque');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../lib/";


# Extract build directory from test properties
ok(-d $props->get_property('torque.build.dir'),"Checking if torque build directory exists") or die("Torque build directory doesn't exist");

# Change directory to build dir
ok(chdir $props->get_property('torque.build.dir'),"Changing directory to " . $props->get_property('torque.build.dir')) or die("Can't change to torque build dir");

# Run setup
runCommand("./torque.setup root") && die("Torque setup script failed");

# Shutdown torque
runCommand("qterm -t quick") && die("Couldn't shut down torque!");
