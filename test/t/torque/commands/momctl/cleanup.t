#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


use CRI::Test;

use Torque::Util qw(
                             list2array
                          );
use Torque::Ctrl        qw(
                             startTorque stopTorque
                          );

# Describe Test
plan('no_plan');
setDesc('Momctl Cleanup');

# Variables
my $mom_cfg_file     = $props->get_property('mom.config.file');
my $mom_cfg_file_bak = $props->get_property('mom.config.file') . ".bak";

# Torque params
my @remote_nodes     = list2array($props->get_property('Torque.Remote.Nodes'));
my $torque_params    = {
                          'remote_moms' => \@remote_nodes
                       };

# Stop Torque
stopTorque($torque_params) 
  or die 'Unable to stop Torque';

# Restore the old configuration file on the local node
eval
  {

  if (-e $mom_cfg_file_bak)
    {

    `cp -f $mom_cfg_file_bak $mom_cfg_file`

    }

  }; # END eval

ok(! $@, "Restoring mom configuration to '$mom_cfg_file'");

# Restore the old configuration on the remote nodes
foreach my $remote_node (@remote_nodes)
  {


  SKIP:
    {

    my $ls_cmd = "ls $mom_cfg_file_bak";
    my %ls     = runCommandSsh($remote_node, $ls_cmd);

    skip("Backup config file '$remote_node:$mom_cfg_file_bak' not found", 1)
      if ($ls{ 'EXIT_CODE' } != 0);

    my $cp_cmd = "cp -f $mom_cfg_file_bak $mom_cfg_file";
    my %cp     = runCommandSsh($remote_node, $cp_cmd);

    ok($cp{ 'EXIT_CODE' } == 0, "Checking if restoring the backup file '$remote_node:$mom_cfg_file_bak' succeeded");

    } # END SKIP:

  } # END foreach my $remote_node (@remote_nodes)

# Restart Torque
startTorque($torque_params)
  or die 'Unable to start Torque';
