#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../../lib/";


use CRI::Test;
use Torque::Ctrl        qw( startTorque 
                            stopTorque 
                            stopPbssched
                          );
use Torque::Test::Utils qw( 
                            list2array             
                          );

# Describe Test
plan('no_plan');
setDesc('Qchkpt Setup');

my @remote_moms    = list2array($props->get_property('torque.remote.nodes'));
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
stopPbssched() 
  or die 'Unable to stop pbs_sched';

###############################################################################
# Restart Torque
###############################################################################
startTorque($torque_params)
  or die 'Unable to start Torque';
