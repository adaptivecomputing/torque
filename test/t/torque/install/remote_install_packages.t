#! /usr/bin/perl 

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
use CRI::Util qw(
                   list2array
                 );

plan('no_plan');
setDesc('Install Torque on Remote Nodes');

# Variables
my $nodes_str      = $props->get_property('Torque.Remote.Nodes');
my @nodes          = list2array($nodes_str);
my $pbs_server     = $props->get_property('Test.Host');
my $pbs_server_loc = $props->get_property('Torque.Home.Dir') . "/server_name";
my $build_dir      = test_lib_loc().'/../..';

pass("No remote nodes given") unless scalar @nodes;

runCommand("cd $build_dir; make packages", test_success_die => 1, msg => 'Making packages to distribute to remote nodes');

opendir DIR, $build_dir
  or die "Unable to open $build_dir: $!";
my @packages = grep { /^torque-package-/ && -f "$build_dir/$_" } readdir(DIR);
closedir DIR;

foreach my $node (@nodes)
{
  runCommand("scp -B $build_dir/torque-package-* $node:/tmp/.", test_success_die => 1, msg => "Moving Packages to Remote Node $node");

  foreach( @packages )
  {
    runCommandSsh($node, "/tmp/$_ --install --verbose", test_success_die => 1);
  }

  # Set the pbs_mom server to trust the pbs_server
  my $cmd = "echo '$pbs_server' > $pbs_server_loc";
  runCommandSsh($node, $cmd, test_success_die => 1);
}
