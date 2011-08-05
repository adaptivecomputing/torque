#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


use CRI::Test;
use Torque::Ctrl        qw( startTorqueClean stopTorque );
use Torque::Util qw( list2array             );

# Describe Test
plan('no_plan');
setDesc('Pbsnodes Setup');

# Torque params
my @remote_nodes  = list2array($props->get_property('Torque.Remote.Nodes'));
my $torque_params = {
                      'remote_moms' => \@remote_nodes
                    };

# Stop Torque
stopTorque($torque_params) 
  or die 'Unable to stop Torque';

# Write the configuration file
my $host = $props->get_property('Test.Host');
my $logevent             = $props->get_property('mom.config.logevent'             );

my $status_update_time   = $props->get_property('mom.config.status_update_time'   );
my $check_poll_time      = $props->get_property('mom.config.check_poll_time'      );

createMomCfg({ body => <<CFG });
# Configuration file for pbsnodes tests
\$pbsserver            $host
\$pbsclient            $host

\$logevent             $logevent

\$restricted           $host

\$status_update_time   $status_update_time
\$check_poll_time      $check_poll_time

CFG

# Restart Torque
startTorqueClean($torque_params)
  or die 'Unable to start Torque';
