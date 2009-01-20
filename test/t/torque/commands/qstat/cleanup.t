#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../../lib/";


use CRI::Test;
use Torque::Test::Utils qw(
                           run_and_check_cmd
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
setDesc('Qstat Cleanup');

# Torque params
my @remote_moms    = list2array($props->get_property('torque.remote.nodes'));
my $torque_params  = {
                     'remote_moms' => \@remote_moms
                     };

###############################################################################
# Stop all jobs
###############################################################################
run_and_check_cmd('qdel -p all');

###############################################################################
# Stop Torque
###############################################################################
stopTorque() 
  or die 'Unable to stop Torque';

stopPbssched() 
  or die 'Unable to stop Pbssched';
