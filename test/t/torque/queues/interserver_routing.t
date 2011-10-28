#!/usr/bin/perl
######################################################
# TRQ-510 - When queue routing between two different
#  servers, make sure jobs get to desired destination
######################################################
use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
use Torque::Ctrl;
use Torque::Util::Qstat qw( qstat_fx );
use Torque::Job::Ctrl qw( qsub runJobs );
plan('no_plan');
setDesc('Test Inter-Server Queue Routing');

my $localhost = $props->get_property('Test.Host');
my ($remote_torque) = split ',', $props->get_property('Torque.Remote.Nodes');
die 'This test requires multiple machines!' unless defined $remote_torque;

my $route_queue = 'iron_rod';

######################
# Setup Environment
######################
stopTorque({ remote_moms => [$remote_torque] });
stopPbsserver({host => $remote_torque});

createMomCfg();
createMomCfg({
    host => $remote_torque,
    attr_vals => {
      pbsserver => $remote_torque,
    },
  });

createPbsserverNodes({host => $remote_torque});

startPbsmom({
    clean => 1,
    node => $remote_torque,
  });
startPbsserverClean({
    host => $remote_torque,
    add_setup_lines => [
      "qmgr -c 's s submit_hosts += $localhost'",
    ],
  });
startTorqueClean({
    pbs_server => {
      add_queues => [
      {
        $route_queue => {
          queue_type => 'Route',
          route_destinations => "batch\@$remote_torque",
          started => 'true',
          enabled => 'true',
        }
      },
      ],
      add_setup_lines => [
        "qmgr -c 's s submit_hosts += $remote_torque'",
      ],
    }
  });

runCommand("qmgr -c 's q $route_queue enabled = true'", test_success_die => 1);

my @jobs;
push @jobs, qsub({ flags => "-q $route_queue" }) for 1 .. 2000;

sleep_diag 3, 'Letting jobs settle...';

my %jhash = qstat_fx({ runcmd_flags => {host => $remote_torque} });

is_deeply([sort keys %jhash], [sort @jobs], 'All jobs migrated to remote server');
