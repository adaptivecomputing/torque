#!/usr/bin/perl

use strict;
use warnings;

use CRI::Test;

use Torque::Test::Utils qw(
                           list2array
                          );

plan('no_plan');
setDesc('Run the ./configure script for BLCR on the remote nodes');

# Variables
my $blrc_prefix        = $props->get_property( 'blcr.home.dir'        );
my $blcr_extracted_dir = $props->get_property( 'blcr.build.dir' );
my $configure_script   = "$blcr_extracted_dir/configure";

my @remote_nodes       = list2array($props->get_property('torque.remote.nodes'));

foreach my $remote_node (@remote_nodes)
  {

  # Make sure the extracted directory exists
  my $ls_cmd = "ls $configure_script";
  my %ls     = runCommandSsh($remote_node, $ls_cmd);
  ok($ls{ 'EXIT_CODE' } == 0, "Checking for the BLCR configure script at '$remote_node:$configure_script'")
    or die "BLCR configure command not found";

  diag("Configuring BLCR for $remote_node...\n");

  # Extract the file
  my $configure_cmd = "cd $blcr_extracted_dir; ./configure --prefix $blrc_prefix";
  my %configure     = runCommandSsh($remote_node, $configure_cmd);

  # Check the configuration results
  $ls_cmd = "ls $blcr_extracted_dir/Makefile";
  %ls     = runCommandSsh($remote_node, $ls_cmd);

  ok($configure{ 'EXIT_CODE' } == 0,                                                   "Checking the exit code for '$configure_cmd'"   );
  ok($ls{ 'EXIT_CODE' } == 0,                                                          "Check for the existance of the 'Makefile'"     );

  } # END foreach my $remote_node (@remote_nodes)
