#!/usr/bin/perl

use strict;
use warnings;

use CRI::Test;
use Torque::Ctrl        qw( startTorque stopTorque );
use Torque::Test::Utils qw( list2array             );

# Describe Test
plan('no_plan');
setDesc('Pbsnodes Setup');

# Torque params
my @remote_nodes  = list2array($props->get_property('torque.remote.nodes'));
my $torque_params = {
                      'remote_moms' => \@remote_nodes
                    };

# Stop Torque
stopTorque($torque_params) 
  or die 'Unable to stop Torque';

# Write the configuration file
my $pbsserver            = $props->get_property('MoabHost'                        );
my $pbsclient            = $props->get_property('MoabHost'                        );
my $restricted           = $props->get_property('MoabHost'                        );
my $logevent             = $props->get_property('mom.config.logevent'             );

my $status_update_time   = $props->get_property('mom.config.status_update_time'   );
my $check_poll_time      = $props->get_property('mom.config.check_poll_time'      );

my $mom_cfg              =<<CFG;
# Configuration file for pbsnodes tests
\$pbsserver            $pbsserver
\$pbsclient            $pbsclient

\$logevent             $logevent

\$restricted           $restricted

\$status_update_time   $status_update_time
\$check_poll_time      $check_poll_time

CFG

my $mom_cfg_file         = $props->get_property('mom.config.file');
my $mom_cfg_file_bak     = $props->get_property('mom.config.file') . ".bak";

eval
  {

  # Backup the old configuration file
  if (-e $mom_cfg_file)
    {

    `cp -f $mom_cfg_file $mom_cfg_file_bak`

    }

  open(MOMCFG, ">$mom_cfg_file")
    or die "Unable to open file '$mom_cfg_file' for writing";
  print MOMCFG $mom_cfg;
  close(MOMCFG);

  }; # END eval

ok($@ eq '', "Writing out mom configuration to '$mom_cfg_file'");

# Restart Torque
startTorque($torque_params)
  or die 'Unable to start Torque';
