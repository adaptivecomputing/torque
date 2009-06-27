#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";

use CRI::Test;

use Torque::Util qw(
                           list2array
                           run_and_check_cmd
                          );

plan('no_plan');
setDesc('Check for BLCR on all nodes');

# Variables
my $local_node   = $props->get_property('Test.Host');
my @remote_nodes = list2array($props->get_property('Torque.Remote.Nodes'));

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
