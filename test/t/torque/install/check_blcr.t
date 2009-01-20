#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../lib/";


use CRI::Test;

use Torque::Test::Utils qw(
                           list2array
                           run_and_check_cmd
                          );

plan('no_plan');
setDesc('Check for BLCR on all nodes');

# Variables
my $local_node   = $props->get_property('MoabHost');
my @remote_nodes = list2array($props->get_property('torque.remote.nodes'));

my $lsmod_cmd;
my %lsmod;
my @blcr_modules = qw(blcr_imports blcr);

# Check the local host
foreach my $blcr_module (@blcr_modules)
  {

  $lsmod_cmd = "lsmod | grep $blcr_module";
  %lsmod     = run_and_check_cmd($lsmod_cmd);
  ok($lsmod{ 'STDOUT' } =~ /${blcr_module}/, "Checking for '$blcr_module' on '$local_node'");

  } # END foreach my $blcr_module (@blcr_modules)


# Check the remote nodes
foreach my $remote_node (@remote_nodes)
  {

  foreach my $blcr_module (@blcr_modules)
    {

    $lsmod_cmd = "lsmod | grep $blcr_module";
    %lsmod     = runCommandSsh($remote_node, $lsmod_cmd);
    ok($lsmod{ 'STDOUT' } =~ /${blcr_module}/, "Checking for '$blcr_module' on '$remote_node'");

    } # END foreach my $blcr_module (@blcr_modules)
 
 } # END foreach my $remote_node (@remote_nodes)

