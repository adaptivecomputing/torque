#!/usr/bin/perl
use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

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
my $q2             = $props->get_property('torque.queue.two');

my $torque_params  = {
  remote_moms => \@remote_moms,
  pbs_server => {
    add_queues => [
    {
      $q2 => {
        started => 'false',
        enabled => 'false',
        'resources_default.walltime' => '1:00:00',
        'resources_default.nodes' => 1,
        'resources_max.cput' => '10:10:10',
        'resources_max.mem' => '2044mb',
        'resources_max.walltime' => '10:10:10',
        'resources_max.nodect' => 10,
        max_running => 2,
      },
    },
    ],
  },
};

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
