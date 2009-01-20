#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";


use CRI::Test;

use Torque::Test::Utils qw( list2array );

plan('no_plan');
setDesc('Load BLCR modules (make insmod) on remote nodes');

# Variables
my $blcr_extracted_dir = $props->get_property( 'blcr.build.dir' );
my $makefile           = "$blcr_extracted_dir/Makefile";

my @remote_nodes       = list2array($props->get_property('torque.remote.nodes'));

foreach my $remote_node (@remote_nodes)
  {

  # Make sure the extracted directory exists
  my $ls_cmd = "ls $makefile";
  my %ls     = runCommandSsh($remote_node, $ls_cmd);
  ok($ls{ 'EXIT_CODE' } == 0, "Checking for the BLCR Makefile at '$remote_node:$makefile'")
    or next;

  # Extract the file
  diag("Running the make insmod command on $remote_node...\n");
  my $make_cmd = "cd $blcr_extracted_dir; make insmod";
  my %make     = runCommandSsh($remote_node, $make_cmd);

  # Check the make results
  ok($make{ 'EXIT_CODE' } == 0, "Checking the exit code for '$make_cmd'"  );

  } # END foreach my $remote_node (@remote_nodes)
