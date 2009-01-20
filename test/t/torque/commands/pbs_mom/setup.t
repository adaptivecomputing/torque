#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


use CRI::Test;
use Torque::Ctrl        qw( startTorque 
                            stopTorque 
                            stopPbssched
                          );
use Torque::Test::Utils qw( 
                            run_and_check_cmd
                            list2array             
                          );

# Describe Test
plan('no_plan');
setDesc('Pbs_mom Setup');

# Torque params
my @remote_moms    = list2array($props->get_property('torque.remote.nodes'));
my $torque_params  = {
                     'remote_moms' => \@remote_moms
                     };

# Mom config parameters
my $pbsserver      = $props->get_property('MoabHost'            );
my $pbsclient      = $props->get_property('MoabHost'            );
my $restricted     = $props->get_property('MoabHost'            );
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

my $mom_recfg =<<RECFG;
# Reconfig file for momctl tests
\$pbsserver            $pbsserver
\$pbsclient            $pbsclient

\$logevent             $logevent

\$restricted           $restricted  

\$check_poll_time      $tmp_check_poll_time
RECFG

eval
  {

  open(MOMRECFG, ">$mom_recfg_file")
    or die "Unable to write to '$mom_recfg_file'";
  print MOMRECFG $mom_recfg;
  close(MOMRECFG);

  }; # END eval

ok(! $@, "Writing out mom reconfiguration to '$mom_recfg_file'");

###############################################################################
# Restart Torque
###############################################################################
startTorque($torque_params)
  or die 'Unable to start Torque';

###############################################################################
# Stop all jobs
###############################################################################
run_and_check_cmd('qdel -p all');
