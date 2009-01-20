#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Test Modules
use CRI::Test;
use Torque::Ctrl                  qw(
                                     startTorque
                                     stopTorque
                                    );
use Torque::Test::Utils           qw( 
                                      list2array
                                    );
use Torque::Test::Pbsnodes::Utils qw( 
                                      test_q_output      
                                    );

# Test Description
plan('no_plan');
setDesc("pbsnodes -l -q");

# Variables
my $cmd;
my $server         = $props->get_property('MoabHost');
my $nodes_str      = $props->get_property('MoabHost');
my @nodes          = list2array($nodes_str);
my $properties_str = $props->get_property('torque.node.args');
my @properties     = list2array($properties_str);
my @states         = qw(offline down);
my $output;
my %pbsnodes;

# Shutdown Torque to produce error messages
stopTorque();

# Test the output of pbsnodes
$cmd         = "pbsnodes -l -q";
%pbsnodes    = runCommand($cmd);
$output      = $pbsnodes{ 'STDERR' };
test_q_output( $output );

# Test the output of pbsnodes <node>
foreach my $node (@nodes)
  {
 
  $cmd         = "pbsnodes -l -q $node";
  %pbsnodes    = runCommand($cmd);
  $output      = $pbsnodes{ 'STDERR' };
  test_q_output( $output );

  } # END foreach my $node (@nodes)

# Test the output of pbsnodes <:property>
foreach my $property (@properties)
  {

  $cmd         = "pbsnodes -l -q :$property";
  %pbsnodes    = runCommand($cmd);
  $output      = $pbsnodes{ 'STDERR' };
  test_q_output( $output );

  } # END foreach my $node (@nodes)

# Test the different states
foreach my $state (@states)
  {

  $cmd         = "pbsnodes -l -q $state";
  %pbsnodes    = runCommand($cmd);
  $output      = $pbsnodes{ 'STDERR' };
  test_q_output( $output );


  } # END foreach my $state (@states)


# Test the different states/node combinatations
foreach my $state (@states)
  {

  foreach my $node (@nodes)
    {

    $cmd         = "pbsnodes -l -q $state $node";
    %pbsnodes    = runCommand($cmd);
    $output      = $pbsnodes{ 'STDERR' };
    test_q_output( $output );

    } # END foreach my $node (@nodes)

  } # END foreach my $state (@states)

# Test the different states/property combinatations
foreach my $state (@states)
  {

  foreach my $property (@properties)
    {

    $cmd         = "pbsnodes -l -q $state $property";
    %pbsnodes    = runCommand($cmd);
    $output      = $pbsnodes{ 'STDERR' };
    test_q_output( $output );

    } # END foreach my $property (@properties)

  } # END foreach my $state (@states)

# Restart Torque
startTorque();
