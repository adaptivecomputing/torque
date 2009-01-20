#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../../lib/";


use CRI::Test;
use Torque::Ctrl        qw(
                            startTorque 
                            stopTorque
                          );
use Torque::Test::Utils qw(
                            list2array
                          );

# Describe Test
plan('no_plan');
setDesc('Pbsnodes Cleanup');

# Torque params
my @remote_moms = list2array($props->get_property('torque.remote.nodes'));
my $torque_params  = {
                     'remote_moms' => \@remote_moms
                  };

###############################################################################
# Stop Torque
###############################################################################
stopTorque($torque_params) 
  or die 'Unable to stop Torque';

###############################################################################
# Restore the old mom configuration file
###############################################################################
my $mom_cfg_file     = $props->get_property('mom.config.file');
my $mom_cfg_file_bak = $props->get_property('mom.config.file') . ".bak";

eval
  {

  if (-e $mom_cfg_file_bak)
    {

    `cp -f $mom_cfg_file_bak $mom_cfg_file`

    }

  }; # END eval

ok(! $@, "Restoring mom configuration to '$mom_cfg_file'");

###############################################################################
# Restore the old node configuration file
###############################################################################
my $node_cfg_path     = $props->get_property('torque.home.dir') . '/server_priv/nodes';
my $node_cfg_path_bak = $node_cfg_path . ".bak";

eval
  {

  if (-e $node_cfg_path_bak)
    {

    `cp -f $node_cfg_path_bak $node_cfg_path`

    }

  }; # END eval

ok(! $@, "Restoring node configuration to '$node_cfg_path'");

###############################################################################
# Restart Torque
###############################################################################
startTorque($torque_params)
  or die 'Unable to start Torque';
