#!/usr/bin/perl

use strict;
use warnings;

use CRI::Test;

use Torque::Test::Utils qw( list2array );

plan('no_plan');
setDesc('Uninstall (make uninstall) BLCR on remote nodes');

# Variables
my $blcr_build_dir = $props->get_property( 'blcr.build.dir' );
my $makefile       = "$blcr_build_dir/Makefile";

my @remote_nodes   = list2array($props->get_property('torque.remote.nodes'));

foreach my $remote_node (@remote_nodes)
  {

  SKIP:
    {

    # Make sure the extracted directory exists
    my $ssh_cmd = "ls $makefile";
    my %ssh     = runCommandSsh($remote_node, $ssh_cmd);
    skip("$remote_node:$makefile not found. Must not be installed", 1) 
      if ($ssh{ 'EXIT_CODE' } != 0);

    # Extract the file
    logMsg("Running the make uninstall command ...\n");
    my $make_cmd = "cd $blcr_build_dir; make uninstall";
    my %make     = runCommandSsh($remote_node, $make_cmd);

    # Check the make uninstall results
    ok($make{ 'EXIT_CODE' } == 0, "Checking the exit code for '$make_cmd'"  );

    }; # END SKIP:

  my $rm_cmd    = "rm -rf $blcr_build_dir";
  my %rm_result = runCommandSsh($remote_node, $rm_cmd);

  cmp_ok($rm_result{ 'EXIT_CODE' }, '==', 0, "Checking the exit code for '$rm_cmd'")
    or diag("STDERR: $rm_result{ 'STDERR' }");

  } # END foreach my $remote_node (@remote_nodes)


