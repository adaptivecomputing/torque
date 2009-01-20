#! /usr/bin/perl 

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";

use CRI::Test;
use CRI::Utils qw(
                   list2array
                 );

plan('no_plan');
setDesc('Install pbs_mom on remote compute nodes');

# Variables
my $nodes_str          = $props->get_property('torque.remote.nodes');
my @nodes              = list2array($nodes_str);
my $remote_install_bat = "$FindBin::Bin/../remote_reinstall.bat";

foreach my $node (@nodes)
  {

  my %ssh = runCommandSsh($node, $remote_install_bat);
  cmp_ok($ssh{ 'EXIT_CODE' }, '==', 0, "Checking exit code of '$node:$remote_install_bat'")
    or die("Unable to install torque on '$node': $ssh{ 'STDERR' }");

  } # END foreach my $node (@nodes)

