#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


use CRI::Test;
use Torque::Ctrl        qw( startTorque 
                            stopTorque 
                            startPbssched
                            stopPbssched
                          );
use Torque::Util qw( 
                            run_and_check_cmd
                            list2array             
                          );

# Describe Test
plan('no_plan');
setDesc('Qalter Setup');

my @remote_moms    = list2array($props->get_property('Torque.Remote.Nodes'));
my $torque_params  = {
                     'remote_moms' => \@remote_moms
                     };

###############################################################################
# Stop Torque
###############################################################################
stopTorque($torque_params) 
  or die 'Unable to stop Torque';

###############################################################################
# Stop pbs_sched
###############################################################################
stopPbssched();

###############################################################################
# Restart Torque
###############################################################################
startTorque($torque_params)
  or die 'Unable to start Torque';
