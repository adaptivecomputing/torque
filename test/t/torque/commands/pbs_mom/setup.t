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
use Torque::Util qw( 
                            run_and_check_cmd
                            list2array             
                          );

# Describe Test
plan('no_plan');
setDesc('Pbs_mom Setup');

# Torque params
my @remote_moms    = list2array($props->get_property('Torque.Remote.Nodes'));
my $torque_params  = {
                     'remote_moms' => \@remote_moms
                     };

# Mom config parameters
my $pbsserver      = $props->get_property('Test.Host'            );
my $pbsclient      = $props->get_property('Test.Host'            );
my $restricted     = $props->get_property('Test.Host'            );
my $logevent       = $props->get_property('mom.config.logevent' );

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
# Create the alternate reconfig file
###############################################################################
my $tmp_check_poll_time = $props->get_property('tmp.mom.config.check_poll_time');
my $mom_recfg_file      = $props->get_property('mom.reconfig.file');

diag("Creating the mom reconfig file '$mom_recfg_file'");

createMomCfg({ mom_cfg_loc => $mom_recfg_file, body => <<RECFG });
# Reconfig file for momctl tests
\$pbsserver            $pbsserver
\$pbsclient            $pbsclient

\$logevent             $logevent

\$restricted           $restricted  

\$check_poll_time      $tmp_check_poll_time
RECFG

###############################################################################
# Restart Torque
###############################################################################
startTorqueClean($torque_params);
