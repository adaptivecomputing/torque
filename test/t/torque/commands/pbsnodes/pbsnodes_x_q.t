#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


# Test Modules
use CRI::Test;
use Torque::Ctrl           qw(
                              startTorque
                              stopTorque
                             );
use Torque::Util    qw( 
                              list2array
                             );
use Torque::Test::Pbsnodes qw( 
                              test_q_output      
                             );

# Test Description
plan('no_plan');
setDesc("pbsnodes -x -q");

# Variables
my $cmd;
my $server         = $props->get_property('Test.Host');
my $nodes_str      = $props->get_property('Test.Host');
my @nodes          = list2array($nodes_str);
my $properties_str = $props->get_property('torque.node.args');
my @properties     = list2array($properties_str);
my $output;
my %pbsnodes;

# Shutdown Torque to produce error messages
stopTorque();

# Test the output of pbsnodes
$cmd         = "pbsnodes -x -q";
%pbsnodes    = runCommand($cmd);
$output      = $pbsnodes{ 'STDERR' };
test_q_output( $output );

# Test the output of pbsnodes <node>
foreach my $node (@nodes)
  {
 
  $cmd         = "pbsnodes -x -q $node";
  %pbsnodes    = runCommand($cmd);
  $output      = $pbsnodes{ 'STDERR' };
  test_q_output( $output );

  } # END foreach my $node (@nodes)

# Test the output of pbsnodes <:property>
foreach my $property (@properties)
  {

  $cmd         = "pbsnodes -x -q :$property";
  %pbsnodes    = runCommand($cmd);
  $output      = $pbsnodes{ 'STDERR' };
  test_q_output( $output );

  } # END foreach my $node (@nodes)

# Restart Torque
startTorque();
