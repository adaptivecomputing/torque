#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


use CRI::Test;
use Torque::Util qw(
                           list2array
                          );
use Torque::Ctrl        qw(
                           startTorque 
                           stopTorque
                           startPbssched
                           stopPbssched
                          );

# Describe Test
plan('no_plan');
setDesc('Qhold Cleanup');

# Torque params
my @remote_moms    = list2array($props->get_property('Torque.Remote.Nodes'));
my $torque_params  = {
                     'remote_moms' => \@remote_moms
                     };

###############################################################################
# Stop Torque
###############################################################################
stopTorque($torque_params) 
  or die 'Unable to stop Torque';

stopPbssched() 
  or die 'Unable to stop Pbssched';
