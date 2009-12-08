#!/usr/bin/perl
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;
use Torque::Ctrl        qw( startTorqueClean
                            stopTorque 
                            stopPbssched
                          );
use Torque::Util qw( list2array );
plan('no_plan');
setDesc('Qstat Setup');

# Torque params
my @remote_moms    = list2array($props->get_property('Torque.Remote.Nodes'));
my $torque_params  = { 'remote_moms' => \@remote_moms };
my $q2             = $props->get_property('torque.queue.two');

###############################################################################
# Stop Torque
###############################################################################
stopTorque($torque_params); 

###############################################################################
# Stop pbs_sched
###############################################################################
stopPbssched();

###############################################################################
# Restart Torque
###############################################################################
startTorqueClean($torque_params);

runCommand("qmgr -c 'create queue $q2'", 'test_success_die' => 1);
runCommand("qmgr -c 'set queue $q2 queue_type = execution'", 'test_success_die' => 1);
runCommand("qmgr -c 'set queue $q2 started = false'", 'test_success_die' => 1);
runCommand("qmgr -c 'set queue $q2 enabled = false'", 'test_success_die' => 1);
runCommand("qmgr -c 'set queue $q2 resources_default.walltime = 1:00:00'", 'test_success_die' => 1);
runCommand("qmgr -c 'set queue $q2 resources_default.nodes = 1'", 'test_success_die' => 1);
runCommand("qmgr -c 'set queue $q2 resources_max.cput = 10:10:10'", 'test_success_die' => 1);
runCommand("qmgr -c 'set queue $q2 resources_max.mem = 2044mb'", 'test_success_die' => 1);
runCommand("qmgr -c 'set queue $q2 resources_max.walltime = 10:10:10'", 'test_success_die' => 1);
runCommand("qmgr -c 'set queue $q2 resources_max.nodect = 10'", 'test_success_die' => 1);
runCommand("qmgr -c 'set queue $q2 max_running = 2'", 'test_success_die' => 1);

sleep_diag 3;
