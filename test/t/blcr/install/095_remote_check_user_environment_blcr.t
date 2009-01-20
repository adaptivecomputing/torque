#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../lib/";


use CRI::Test;

use Torque::Test::Utils qw( list2array );

plan('no_plan');
setDesc('Check the user environment on remote nodes');

my @remote_nodes = list2array($props->get_property('torque.remote.nodes'));

foreach my $remote_node (@remote_nodes)
  {

  # Commands
  my $path_cmd            = "echo \$PATH";
  my $manpath_cmd         = "echo \$MANPATH";
  my $ld_library_path_cmd = "echo \$LD_LIBRARY_PATH";

  my %path_result            = runCommandSsh($remote_node, $path_cmd);
#  my %manpath_result         = runCommandSsh($remote_node, $manpath_cmd);
  my %ld_library_path_result = runCommandSsh($remote_node, $ld_library_path_cmd);

  # Variables
  my $path                 = $path_result{ 'STDOUT' };
#  my $manpath              = $manpath_result{ 'STDOUT' };
  my $ld_library_path      = $ld_library_path_result{ 'STDOUT' };

  my $prefix               = $props->get_property( 'blcr.home.dir' );

  my $blcr_path            = $prefix . "bin";
#  my $blcr_manpath         = $prefix . "man";
  my $blcr_ld_library_path = $prefix . "lib";

  # Check for the variables.  Unfortunately we can only look for the variables and not set them
  ok($path            =~ /${blcr_path}/,            "Checking for '$blcr_path' in the PATH environment variable"                      );
#  ok($manpath         =~ /${blcr_manpath}/,         "Checking for '$blcr_manpath' in the MANPATH environment variable"                );
  ok($ld_library_path =~ /${blcr_ld_library_path}/, "Checking for '$blcr_ld_library_path' in the LD_LIBRARY_PATH environment variable");

  } # END foreach my $remote_node (@remote_nodes)
